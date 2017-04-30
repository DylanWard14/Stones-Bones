// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "SkellyDefense.h"
#include "SkellyDefenseCharacter.h"
#include "SkellyDefenseProjectile.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "MotionControllerComponent.h"
#include "Engine.h"
#include "Net/UnrealNetwork.h"

static FName WeaponFireTraceIdent = FName(TEXT("WeaponTrace"));
#define COLLISION_WEAPON		ECC_GameTraceChannel1

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ASkellyDefenseCharacter

ASkellyDefenseCharacter::ASkellyDefenseCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	// Create VR Controllers.
	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->Hand = EControllerHand::Right;
	R_MotionController->SetupAttachment(RootComponent);
	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(RootComponent);

	// Create a gun and attach it to the right-hand VR controller.
	// Create a gun mesh component
	VR_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VR_Gun"));
	VR_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	VR_Gun->bCastDynamicShadow = false;
	VR_Gun->CastShadow = false;
	VR_Gun->SetupAttachment(R_MotionController);
	VR_Gun->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	VR_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("VR_MuzzleLocation"));
	VR_MuzzleLocation->SetupAttachment(VR_Gun);
	VR_MuzzleLocation->SetRelativeLocation(FVector(0.000004, 53.999992, 10.000000));
	VR_MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));		// Counteract the rotation of the VR gun model.

	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;
}

void ASkellyDefenseCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	if (bUsingMotionControllers)
	{
		VR_Gun->SetHiddenInGame(false, true);
		Mesh1P->SetHiddenInGame(true, true);
	}
	else
	{
		VR_Gun->SetHiddenInGame(true, true);
		Mesh1P->SetHiddenInGame(false, true);
	}

	CurrentAmmo = MaxAmmo;
	
	//GetWorld()->GetTimerManager().SetTimer(LoopTimerHandle, this, &ASkellyDefenseCharacter::OnFire, 1.0f, true);
}

//////////////////////////////////////////////////////////////////////////
// Input

void ASkellyDefenseCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	//InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ASkellyDefenseCharacter::TouchStarted);
	if (EnableTouchscreenMovement(PlayerInputComponent) == false)
	{
		PlayerInputComponent->BindAction("BeginFire", IE_Pressed, this, &ASkellyDefenseCharacter::OnLeftMouseDown);
		PlayerInputComponent->BindAction("EndFire", IE_Released , this, &ASkellyDefenseCharacter::OnLeftMouseUp);

		PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ASkellyDefenseCharacter::StartReload);
	}

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ASkellyDefenseCharacter::OnResetVR);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASkellyDefenseCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASkellyDefenseCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ASkellyDefenseCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ASkellyDefenseCharacter::LookUpAtRate);
}

/*void ASkellyDefenseCharacter::OnFire()
{
	// try and fire a projectile
	if (ProjectileClass != NULL)
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			if (bUsingMotionControllers)
			{
				const FRotator SpawnRotation = VR_MuzzleLocation->GetComponentRotation();
				const FVector SpawnLocation = VR_MuzzleLocation->GetComponentLocation();
				World->SpawnActor<ASkellyDefenseProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
			}
			else
			{
				const FRotator SpawnRotation = GetControlRotation();
				// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
				const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

				//Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

				// spawn the projectile at the muzzle
				World->SpawnActor<ASkellyDefenseProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
			}
		}
	}

	// try and play the sound if specified
	if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != NULL)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}*/

void ASkellyDefenseCharacter::StartReload_Implementation()
{
	GetWorldTimerManager().SetTimer(ReloadTimer, this, &ASkellyDefenseCharacter::Reload, ReloadDelay, false);
	Reloading = true;
}

bool ASkellyDefenseCharacter::StartReload_Validate()
{
	return true;
}

void ASkellyDefenseCharacter::Reload_Implementation()
{
		//if (Reloading)
		//{
			////ReloadTimer += DeltaSeconds;
			//if (ReloadTimer >= ReloadDelay)
			//{
				Reloading = false;
				CurrentAmmo = MaxAmmo;
			//	Reloading = false;
			//}
		//}
}

bool ASkellyDefenseCharacter::Reload_Validate()
{
	return true;
}


void ASkellyDefenseCharacter::OnLeftMouseDown()
{
	LeftMouseDown = true;
}

void ASkellyDefenseCharacter::OnLeftMouseUp()
{
	LeftMouseDown = false;
}

void ASkellyDefenseCharacter::OnFirePlayAnim()
{
		if (!isDead && CanShoot)
		{
			// Play a sound if there is one
			if (FireSound != NULL)
			{
				UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
			}

			// try and play a firing animation if specified
			if (FireAnimation != NULL)
			{
				// Get the animation object for the arms mesh
				UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
				if (AnimInstance != NULL)
				{
					AnimInstance->Montage_Play(FireAnimation, 1.f);
				}
			}
		}
	
}

void ASkellyDefenseCharacter::OnFire_Implementation()
{
		if (!isDead && CanShoot)
		{
			if (!bShootsProjectile)
			{
				// Now send a trace from the end of our gun to see if we should hit anything
				APlayerController* PlayerController = Cast<APlayerController>(GetController());

				// Calculate the direction of fire and the start location for trace
				FVector CamLoc;
				FRotator CamRot;
				PlayerController->GetPlayerViewPoint(CamLoc, CamRot);
				const FVector ShootDir = CamRot.Vector();

				FVector StartTrace = FVector::ZeroVector;
				if (PlayerController)
				{
					FRotator UnusedRot;
					PlayerController->GetPlayerViewPoint(StartTrace, UnusedRot);

					// Adjust trace so there is nothing blocking the ray between the camera and the pawn, and calculate distance from adjusted start
					StartTrace = StartTrace + ShootDir * ((GetActorLocation() - StartTrace) | ShootDir);
				}

				// Calculate endpoint of trace
				const FVector EndTrace = StartTrace + ShootDir * WeaponRange;

				// Check for impact
				const FHitResult Impact = WeaponTrace(StartTrace, EndTrace);

				// Deal with impact
				AActor* DamagedActor = Impact.GetActor();
				UPrimitiveComponent* DamagedComponent = Impact.GetComponent();

				// If we hit an actor, with a component that is simulating physics, apply an impulse
				if ((DamagedActor != NULL) && (DamagedActor != this) && (DamagedComponent != NULL) && DamagedComponent->IsSimulatingPhysics())
				{
					UE_LOG(LogTemp, Warning, TEXT("Hit"))
						DamagedComponent->AddImpulseAtLocation(ShootDir*WeaponDamage, Impact.Location);
				}

				if (DamagedActor != NULL && DamagedActor->ActorHasTag("Enemy"))
				{
					UGameplayStatics::ApplyDamage(DamagedActor, DamageOutput, GetInstigatorController(), this, UDamageType::StaticClass());
				}
				CurrentAmmo--;
				ShootTimer = 0.0f;
				CanShoot = false;
			}

			else if (bShootsProjectile)
			{
				if (ProjectileClass != NULL)
				{
					UWorld* const World = GetWorld();
					if (World != NULL)
					{
						if (bUsingMotionControllers)
						{
							const FRotator SpawnRotation = VR_MuzzleLocation->GetComponentRotation();
							const FVector SpawnLocation = VR_MuzzleLocation->GetComponentLocation();
							World->SpawnActor<ASkellyDefenseProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
						}
						else
						{
							const FRotator SpawnRotation = GetControlRotation();
							// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
							const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

							//Set Spawn Collision Handling Override
							FActorSpawnParameters ActorSpawnParams;
							ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

							// spawn the projectile at the muzzle
							World->SpawnActor<ASkellyDefenseProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
							CurrentAmmo--;
							ShootTimer = 0.0f;
							CanShoot = false;
						}
					}
				}
			}
		}
}

bool ASkellyDefenseCharacter::OnFire_Validate()
{
	return true;
}

void ASkellyDefenseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ShootTimer += DeltaTime;

	if (ShootTimer >= FireDelay && CanShoot == false && CurrentAmmo > 0 && !Reloading)
	{
		ShootTimer = FireDelay;
		CanShoot = true;
	}
	else if (ShootTimer < FireDelay || CurrentAmmo < 0 || Reloading)
	{
		CanShoot = false;
	}

	if (LeftMouseDown)
	{
		OnFire();
		OnFirePlayAnim();
	}

	CheckIfDead(DeltaTime);

	//Reload(DeltaTime);
}

void ASkellyDefenseCharacter::CheckIfDead(float DeltaTime)
{
	if (Health <= 0)
	{
		isDead = true;
		SetActorEnableCollision(false);
		SetActorHiddenInGame(true);
		if (!Respawning)
		{
			Respawning = true;
			respawnTimer = 0;
		}
		respawnTimer += DeltaTime;

		if (respawnTimer >= 30)
		{
			Health = 100;
		}
	}
	else
	{
		if (isDead && Respawning)
		{
			isDead = false;
			Respawning = false;
			SetActorEnableCollision(true);
			SetActorHiddenInGame(false);
			FVector spawnLocation = FVector(0, 0, 260);
			SetActorLocation(spawnLocation);
		}
	}
}

FHitResult ASkellyDefenseCharacter::WeaponTrace(const FVector& StartTrace, const FVector& EndTrace) const
{
	// Perform trace to retrieve hit info
	FCollisionQueryParams TraceParams(WeaponFireTraceIdent, true, Instigator);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, COLLISION_WEAPON, TraceParams);
	/*DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor(255, 0, 0),
		false,
		0.f,
		0.f,
		10.f);*/


	return Hit;
}

void ASkellyDefenseCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ASkellyDefenseCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void ASkellyDefenseCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = false;
}

//Commenting this section out to be consistent with FPS BP template.
//This allows the user to turn without using the right virtual joystick

//void ASkellyDefenseCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
//	{
//		if (TouchItem.bIsPressed)
//		{
//			if (GetWorld() != nullptr)
//			{
//				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
//				if (ViewportClient != nullptr)
//				{
//					FVector MoveDelta = Location - TouchItem.Location;
//					FVector2D ScreenSize;
//					ViewportClient->GetViewportSize(ScreenSize);
//					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
//					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.X * BaseTurnRate;
//						AddControllerYawInput(Value);
//					}
//					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.Y * BaseTurnRate;
//						AddControllerPitchInput(Value);
//					}
//					TouchItem.Location = Location;
//				}
//				TouchItem.Location = Location;
//			}
//		}
//	}
//}

void ASkellyDefenseCharacter::MoveForward(float Value)
{
	if (!isDead && Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ASkellyDefenseCharacter::MoveRight(float Value)
{
	if (!isDead && Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ASkellyDefenseCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ASkellyDefenseCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool ASkellyDefenseCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	bool bResult = false;
	if (FPlatformMisc::GetUseVirtualJoysticks() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		bResult = true;
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ASkellyDefenseCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &ASkellyDefenseCharacter::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ASkellyDefenseCharacter::TouchUpdate);
	}
	return bResult;
}

void ASkellyDefenseCharacter::OnTakeDamage(float Damage)
{
	if (HasAuthority())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Calculating health"));
		Health = Health - Damage;
	}

	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Not Authority"));
	}
}

float ASkellyDefenseCharacter::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	//float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	UE_LOG(LogTemp, Warning, TEXT("Damage = %f"), DamageAmount)
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Taking damage via script"));
	OnTakeDamage(DamageAmount);
	return DamageAmount;
}


void ASkellyDefenseCharacter::UpdateHUD_Implementation()
{
	BP_UpdateHUD();
}

// needed to replicate variables
void ASkellyDefenseCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


	DOREPLIFETIME(ASkellyDefenseCharacter, Health);
	DOREPLIFETIME(ASkellyDefenseCharacter, CurrentAmmo);
	//DOREPLIFETIME(ASkellyDefenseCharacter, DamageOutput);
}
