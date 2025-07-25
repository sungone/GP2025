#include "Network/GPGameInstance.h"
#include "Network/GPNetworkManager.h"
#include "Kismet/GameplayStatics.h"
#include "GPUtils.h"
#include "Network/GPNavMeshExporter.h"

#if PLATFORM_ANDROID
#include "AndroidPermissionFunctionLibrary.h"
#endif

void UGPGameInstance::Init()
{
	Super::Init();
	NetworkMgr = GetSubsystem<UGPNetworkManager>();
	NetworkMgr->ConnectToServer();

#if PLATFORM_ANDROID
	RequestAndroidPermissions();
#endif
}

void UGPGameInstance::Shutdown()
{
	NetworkMgr->DisconnectFromServer();
	Super::Shutdown();
}

bool UGPGameInstance::SaveNavData(bool IsSave)
{
#if !PLATFORM_ANDROID
	if (IsSave)
		return GPNavMeshExporter::ExportNavMesh(GetWorld(), TEXT("NavMeshData.json"));
#endif
    return false;
}

void UGPGameInstance::ChangeZoenRequest(FString LevelName)
{
	auto GetZoneName = [](const FString& InLevelName) -> ZoneType {
		if (InLevelName == "tip") return ZoneType::TIP;
		if (InLevelName == "TUK") return ZoneType::TUK;
		if (InLevelName == "E") return ZoneType::E;
		if (InLevelName == "gym") return ZoneType::GYM;
		if (InLevelName == "industry") return ZoneType::INDUSTY;
		return ZoneType::NONE;
		};
	
	ZoneType TargetZone = GetZoneName(LevelName);
	if (TargetZone == ZoneType::NONE)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GameInstance] Invalid Zone Name: %s"), *LevelName);
		return;
	}

	if (!IsValid(NetworkMgr))
	{
		UE_LOG(LogTemp, Error, TEXT("[GameInstance] NetworkMgr is null! Cannot change zone."));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[GameInstance] Sending Zone Change Request to: %s"), *LevelName);
	NetworkMgr->SendMyZoneChangePacket(TargetZone);
}

void UGPGameInstance::NextQuestRequest()
{
	if (!NetworkMgr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Network Manager is not initialized."));
		return;
	}
	NetworkMgr->SendMyCompleteQuest();
}


#if PLATFORM_ANDROID
	void UGPGameInstance::RequestAndroidPermissions()
	{
        TArray<FString> Permissions;

        Permissions.Add("android.permission.READ_MEDIA_IMAGES");
        Permissions.Add("android.permission.READ_MEDIA_VIDEO");
        Permissions.Add("android.permission.READ_MEDIA_AUDIO");

        Permissions.Add("android.permission.READ_EXTERNAL_STORAGE");
        Permissions.Add("android.permission.WRITE_EXTERNAL_STORAGE");
        Permissions.Add("android.permission.MANAGE_EXTERNAL_STORAGE");

        Permissions.Add("android.permission.POST_NOTIFICATIONS");

        Permissions.Add("android.permission.INTERNET");
        Permissions.Add("android.permission.ACCESS_NETWORK_STATE");
        Permissions.Add("android.permission.ACCESS_WIFI_STATE");

        Permissions.Add("android.permission.BLUETOOTH_SCAN");
        Permissions.Add("android.permission.BLUETOOTH_CONNECT");
        Permissions.Add("android.permission.BLUETOOTH_ADVERTISE");

        Permissions.Add("android.permission.VIBRATE");
        Permissions.Add("android.permission.SCHEDULE_EXACT_ALARM");

        Permissions.Add("android.permission.FOREGROUND_SERVICE");

        Permissions.Add("android.permission.RECORD_AUDIO");

        UAndroidPermissionFunctionLibrary::AcquirePermissions(Permissions);
	}
#endif

