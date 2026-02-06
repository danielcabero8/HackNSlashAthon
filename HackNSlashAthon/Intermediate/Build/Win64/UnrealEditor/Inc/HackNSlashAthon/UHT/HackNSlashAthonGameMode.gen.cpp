// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "HackNSlashAthon/HackNSlashAthonGameMode.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeHackNSlashAthonGameMode() {}

// Begin Cross Module References
ENGINE_API UClass* Z_Construct_UClass_AGameModeBase();
HACKNSLASHATHON_API UClass* Z_Construct_UClass_AHackNSlashAthonGameMode();
HACKNSLASHATHON_API UClass* Z_Construct_UClass_AHackNSlashAthonGameMode_NoRegister();
UPackage* Z_Construct_UPackage__Script_HackNSlashAthon();
// End Cross Module References

// Begin Class AHackNSlashAthonGameMode
void AHackNSlashAthonGameMode::StaticRegisterNativesAHackNSlashAthonGameMode()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(AHackNSlashAthonGameMode);
UClass* Z_Construct_UClass_AHackNSlashAthonGameMode_NoRegister()
{
	return AHackNSlashAthonGameMode::StaticClass();
}
struct Z_Construct_UClass_AHackNSlashAthonGameMode_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "HideCategories", "Info Rendering MovementReplication Replication Actor Input Movement Collision Rendering HLOD WorldPartition DataLayers Transformation" },
		{ "IncludePath", "HackNSlashAthonGameMode.h" },
		{ "ModuleRelativePath", "HackNSlashAthonGameMode.h" },
		{ "ShowCategories", "Input|MouseInput Input|TouchInput" },
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AHackNSlashAthonGameMode>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_AHackNSlashAthonGameMode_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_AGameModeBase,
	(UObject* (*)())Z_Construct_UPackage__Script_HackNSlashAthon,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_AHackNSlashAthonGameMode_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_AHackNSlashAthonGameMode_Statics::ClassParams = {
	&AHackNSlashAthonGameMode::StaticClass,
	"Game",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	nullptr,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	0,
	0,
	0x008802ACu,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_AHackNSlashAthonGameMode_Statics::Class_MetaDataParams), Z_Construct_UClass_AHackNSlashAthonGameMode_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_AHackNSlashAthonGameMode()
{
	if (!Z_Registration_Info_UClass_AHackNSlashAthonGameMode.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_AHackNSlashAthonGameMode.OuterSingleton, Z_Construct_UClass_AHackNSlashAthonGameMode_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_AHackNSlashAthonGameMode.OuterSingleton;
}
template<> HACKNSLASHATHON_API UClass* StaticClass<AHackNSlashAthonGameMode>()
{
	return AHackNSlashAthonGameMode::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(AHackNSlashAthonGameMode);
AHackNSlashAthonGameMode::~AHackNSlashAthonGameMode() {}
// End Class AHackNSlashAthonGameMode

// Begin Registration
struct Z_CompiledInDeferFile_FID_Users_salva_Documents_Unreal_Projects_HackNSlashAthon_HackNSlashAthon_Source_HackNSlashAthon_HackNSlashAthonGameMode_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_AHackNSlashAthonGameMode, AHackNSlashAthonGameMode::StaticClass, TEXT("AHackNSlashAthonGameMode"), &Z_Registration_Info_UClass_AHackNSlashAthonGameMode, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(AHackNSlashAthonGameMode), 1325509932U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_salva_Documents_Unreal_Projects_HackNSlashAthon_HackNSlashAthon_Source_HackNSlashAthon_HackNSlashAthonGameMode_h_3905506926(TEXT("/Script/HackNSlashAthon"),
	Z_CompiledInDeferFile_FID_Users_salva_Documents_Unreal_Projects_HackNSlashAthon_HackNSlashAthon_Source_HackNSlashAthon_HackNSlashAthonGameMode_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_salva_Documents_Unreal_Projects_HackNSlashAthon_HackNSlashAthon_Source_HackNSlashAthon_HackNSlashAthonGameMode_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
