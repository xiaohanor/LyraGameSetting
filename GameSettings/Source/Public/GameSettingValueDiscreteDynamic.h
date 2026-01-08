// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameSettingValueDiscrete.h"

#include "GameSettingValueDiscreteDynamic.generated.h"

#define UE_API GAMESETTINGS_API

class FGameSettingDataSource;
enum class EGameSettingChangeReason : uint8;

struct FContentControlsRules;

//////////////////////////////////////////////////////////////////////////
// UGameSettingValueDiscreteDynamic
//////////////////////////////////////////////////////////////////////////

UCLASS(MinimalAPI)
class UGameSettingValueDiscreteDynamic : public UGameSettingValueDiscrete
{
	GENERATED_BODY()
	
public:
	UE_API UGameSettingValueDiscreteDynamic();

	UFUNCTION(BlueprintCallable, Category="GameSetting|Dynamic")
	UE_API void SetDynamicGetterPath(const TArray<FString>& InGetterPath);

	UFUNCTION(BlueprintCallable, Category="GameSetting|Dynamic")
	UE_API void SetDynamicSetterPath(const TArray<FString>& InSetterPath);

	/** UGameSettingValue */
	UE_API virtual void Startup() override;
	UE_API virtual void StoreInitial() override;
	UE_API virtual void ResetToDefault() override;
	UE_API virtual void RestoreToInitial() override;

	/** UGameSettingValueDiscrete */
	UE_API virtual void SetDiscreteOptionByIndex(int32 Index) override;
	UE_API virtual int32 GetDiscreteOptionIndex() const override;
	UE_API virtual int32 GetDiscreteOptionDefaultIndex() const override;
	UE_API virtual TArray<FText> GetDiscreteOptions() const override;

	/** UGameSettingValueDiscreteDynamic */
	UE_API void SetDynamicGetter(const TSharedRef<FGameSettingDataSource>& InGetter);
	UE_API void SetDynamicSetter(const TSharedRef<FGameSettingDataSource>& InSetter);
	UFUNCTION(BlueprintCallable, Category="GameSetting|Dynamic")
	UE_API void SetDefaultValueFromString(FString InOptionValue);
	UFUNCTION(BlueprintCallable, Category="GameSetting|Dynamic")
	UE_API void AddDynamicOption(FString InOptionValue, FText InOptionText);
	UFUNCTION(BlueprintCallable, Category="GameSetting|Dynamic")
	UE_API void RemoveDynamicOption(FString InOptionValue);
	UFUNCTION(BlueprintPure, Category="GameSetting|Dynamic")
	UE_API TArray<FString> GetOptionValues() const;
	UE_API const TArray<FString>& GetDynamicOptions();
	UFUNCTION(BlueprintPure, Category="GameSetting|Dynamic")
	UE_API bool HasDynamicOption(const FString& InOptionValue) const;

	UFUNCTION(BlueprintPure, Category="GameSetting|Dynamic")
	UE_API FString GetValueAsString() const;
	UFUNCTION(BlueprintCallable, Category="GameSetting|Dynamic")
	UE_API void SetValueFromString(FString InStringValue);

protected:
	UE_API void SetValueFromString(FString InStringValue, EGameSettingChangeReason Reason);

	/** UGameSettingValue */
	UE_API virtual void OnInitialized() override;

	UE_API void OnDataSourcesReady();

	UE_API bool AreOptionsEqual(const FString& InOptionA, const FString& InOptionB) const;

protected:
	TSharedPtr<FGameSettingDataSource> Getter;
	TSharedPtr<FGameSettingDataSource> Setter;

	TOptional<FString> DefaultValue;
	FString InitialValue;

	TArray<FString> OptionValues;
	TArray<FText> OptionDisplayTexts;
};

//////////////////////////////////////////////////////////////////////////
// UGameSettingValueDiscreteDynamic_Bool
//////////////////////////////////////////////////////////////////////////

UCLASS(MinimalAPI)
class UGameSettingValueDiscreteDynamic_Bool : public UGameSettingValueDiscreteDynamic
{
	GENERATED_BODY()

public:
	UE_API UGameSettingValueDiscreteDynamic_Bool();

public:
	UFUNCTION(BlueprintCallable, Category="GameSetting|Dynamic")
	UE_API void SetDefaultValue(bool Value);

	UFUNCTION(BlueprintCallable, Category="GameSetting|Dynamic")
	UE_API void SetTrueText(const FText& InText);
	UFUNCTION(BlueprintCallable, Category="GameSetting|Dynamic")
	UE_API void SetFalseText(const FText& InText);

#if !UE_BUILD_SHIPPING
	void SetTrueText(const FString& Value) { SetTrueText(FText::FromString(Value)); }
	void SetFalseText(const FString& Value) { SetFalseText(FText::FromString(Value)); }
#endif
};

//////////////////////////////////////////////////////////////////////////
// UGameSettingValueDiscreteDynamic_Number
//////////////////////////////////////////////////////////////////////////

UCLASS(MinimalAPI)
class UGameSettingValueDiscreteDynamic_Number : public UGameSettingValueDiscreteDynamic
{
	GENERATED_BODY()

public:
	UE_API UGameSettingValueDiscreteDynamic_Number();

public:
	template<typename NumberType>
	void SetDefaultValue(NumberType InValue)
	{
		SetDefaultValueFromString(LexToString(InValue));
	}

	template<typename NumberType>
	void AddOption(NumberType InValue, const FText& InOptionText)
	{
		AddDynamicOption(LexToString(InValue), InOptionText);
	}

	template<typename NumberType>
	NumberType GetValue() const
	{
		const FString ValueString = GetValueAsString();

		NumberType OutValue;
		LexFromString(OutValue, *ValueString);

		return OutValue;
	}

	template<typename NumberType>
	void SetValue(NumberType InValue)
	{
		SetValueFromString(LexToString(InValue));
	}

protected:
	/** UGameSettingValue */
	UE_API virtual void OnInitialized() override;
};

//////////////////////////////////////////////////////////////////////////
// UGameSettingValueDiscreteDynamic_Enum
//////////////////////////////////////////////////////////////////////////

UCLASS(MinimalAPI)
class UGameSettingValueDiscreteDynamic_Enum : public UGameSettingValueDiscreteDynamic
{
	GENERATED_BODY()

public:
	UE_API UGameSettingValueDiscreteDynamic_Enum();

public:
	template<typename EnumType>
	void SetDefaultValue(EnumType InEnumValue)
	{
		const FString StringValue = StaticEnum<EnumType>()->GetNameStringByValue((int64)InEnumValue);
		SetDefaultValueFromString(StringValue);
	}

	template<typename EnumType>
	void AddEnumOption(EnumType InEnumValue, const FText& InOptionText)
	{
		const FString StringValue = StaticEnum<EnumType>()->GetNameStringByValue((int64)InEnumValue);
		AddDynamicOption(StringValue, InOptionText);
	}

	template<typename EnumType>
	EnumType GetValue() const
	{
		const FString Value = GetValueAsString();
		return (EnumType)StaticEnum<EnumType>()->GetValueByNameString(Value);
	}

	template<typename EnumType>
	void SetValue(EnumType InEnumValue)
	{
		const FString StringValue = StaticEnum<EnumType>()->GetNameStringByValue((int64)InEnumValue);
		SetValueFromString(StringValue);
	}

protected:
	/** UGameSettingValue */
	UE_API virtual void OnInitialized() override;
};

//////////////////////////////////////////////////////////////////////////
// UGameSettingValueDiscreteDynamic_Color
//////////////////////////////////////////////////////////////////////////

UCLASS(MinimalAPI)
class UGameSettingValueDiscreteDynamic_Color : public UGameSettingValueDiscreteDynamic
{
	GENERATED_BODY()

public:
	UE_API UGameSettingValueDiscreteDynamic_Color();

public:
	void SetDefaultValue(FLinearColor InColor)
	{
		SetDefaultValueFromString(InColor.ToString());
	}

	void AddColorOption(FLinearColor InColor)
	{
		const FColor SRGBColor = InColor.ToFColor(true);
		AddDynamicOption(InColor.ToString(), FText::FromString(FString::Printf(TEXT("#%02X%02X%02X"), SRGBColor.R, SRGBColor.G, SRGBColor.B)));
	}

	FLinearColor GetValue() const
	{
		const FString Value = GetValueAsString();
		
		FLinearColor ColorValue;
		bool bSuccess = ColorValue.InitFromString(Value);
		ensure(bSuccess);

		return ColorValue;
	}

	void SetValue(FLinearColor InColor)
	{
		SetValueFromString(InColor.ToString());
	}
};

//////////////////////////////////////////////////////////////////////////
// UGameSettingValueDiscreteDynamic_Vector2D
//////////////////////////////////////////////////////////////////////////

UCLASS(MinimalAPI)
class UGameSettingValueDiscreteDynamic_Vector2D : public UGameSettingValueDiscreteDynamic
{
	GENERATED_BODY()

public:

	UGameSettingValueDiscreteDynamic_Vector2D() { }

	void SetDefaultValue(const FVector2D& InValue)
	{	
		SetDefaultValueFromString(InValue.ToString());
	}

	FVector2D GetValue() const
	{
		FVector2D ValueVector;
		ValueVector.InitFromString(GetValueAsString());
		return ValueVector;
	}

	void SetValue(const FVector2D& InValue)
	{
		SetValueFromString(InValue.ToString());
	}
};

#undef UE_API
