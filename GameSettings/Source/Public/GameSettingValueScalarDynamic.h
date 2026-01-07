// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameSettingValueScalar.h"

#include "GameSettingValueScalarDynamic.generated.h"

#define UE_API GAMESETTINGS_API

struct FNumberFormattingOptions;

class FGameSettingDataSource;
class UObject;

//////////////////////////////////////////////////////////////////////////
// UGameSettingValueScalarDynamic
//////////////////////////////////////////////////////////////////////////

typedef TFunction<FText(double SourceValue, double NormalizedValue)> FSettingScalarFormatFunction;

UENUM(BlueprintType)
enum class EGameSettingScalarDynamicFormat : uint8
{
	/** Display the source value without additional formatting. */
	Raw,
	/** Display the source value with one fractional digit. */
	RawOneDecimal,
	/** Display the source value with two fractional digits. */
	RawTwoDecimals,
	/** Display normalized 0-1 value as a percentage with no decimals. */
	ZeroToOnePercent,
	/** Display normalized 0-1 value as a percentage with one decimal. */
	ZeroToOnePercent_OneDecimal,
	/** Display source value multiplied by 100 as a percentage. */
	SourceAsPercent1,
	/** Display source value as percentage assuming it is already scaled by 100. */
	SourceAsPercent100,
	/** Display source value as an integer with no decimals. */
	SourceAsInteger,
};

UCLASS(MinimalAPI)
class UGameSettingValueScalarDynamic : public UGameSettingValueScalar
{
	GENERATED_BODY()

public:
	static UE_API FSettingScalarFormatFunction Raw;
	static UE_API FSettingScalarFormatFunction RawOneDecimal;
	static UE_API FSettingScalarFormatFunction RawTwoDecimals;
	static UE_API FSettingScalarFormatFunction ZeroToOnePercent;
	static UE_API FSettingScalarFormatFunction ZeroToOnePercent_OneDecimal;
	static UE_API FSettingScalarFormatFunction SourceAsPercent1;
	static UE_API FSettingScalarFormatFunction SourceAsPercent100;
	static UE_API FSettingScalarFormatFunction SourceAsInteger;
private:
	static const FNumberFormattingOptions& GetOneDecimalFormattingOptions();
	
public:
	UE_API UGameSettingValueScalarDynamic();

	/** UGameSettingValue */
	UE_API virtual void Startup() override;
	UE_API virtual void StoreInitial() override;
	UE_API virtual void ResetToDefault() override;
	UE_API virtual void RestoreToInitial() override;

	/** UGameSettingValueScalar */
	UE_API virtual TOptional<double> GetDefaultValue() const override;
	UE_API virtual void SetValue(double Value, EGameSettingChangeReason Reason = EGameSettingChangeReason::Change) override;
	UE_API virtual double GetValue() const override;
	UE_API virtual TRange<double> GetSourceRange() const override;
	UE_API virtual double GetSourceStep() const override;
	UE_API virtual FText GetFormattedText() const override;

	/** UGameSettingValueDiscreteDynamic */
	UE_API void SetDynamicGetter(const TSharedRef<FGameSettingDataSource>& InGetter);
	UE_API void SetDynamicSetter(const TSharedRef<FGameSettingDataSource>& InSetter);
	UFUNCTION(BlueprintCallable, Category = "GameSetting|DataSource")
	UE_API void SetDynamicGetterPath(const TArray<FString>& InGetterPath);
	UFUNCTION(BlueprintCallable, Category = "GameSetting|DataSource")
	UE_API void SetDynamicSetterPath(const TArray<FString>& InSetterPath);
	UFUNCTION(BlueprintCallable, Category = "GameSetting")
	UE_API void SetDefaultValue(double InValue);

	/** Sets a custom formatting function for displaying the scalar value. */
	UE_API void SetDisplayFormat(FSettingScalarFormatFunction InDisplayFormat);
	/** Selects one of the built-in formatting presets for displaying the scalar value. */
	UFUNCTION(BlueprintCallable, Category = "GameSetting")
	UE_API void SetDisplayFormatPreset(EGameSettingScalarDynamicFormat InPreset);
	
	/** Sets the source range and step size used for the scalar value. */
	UE_API void SetSourceRangeAndStep(const TRange<double>& InRange, double InSourceStep);
	UFUNCTION(BlueprintCallable, Category = "GameSetting")
	UE_API void SetSourceRangeAndStepValues(double InMinimum, double InMaximum, double InSourceStep);
	
	/**
	 * The SetSourceRangeAndStep defines the actual range the numbers could move in, but often
	 * the true minimum for the user is greater than the minimum source range, so for example, the range
	 * of some slider might be 0..100, but you want to restrict the slider so that while it shows 
	 * a bar that travels from 0 to 100, the user can't set anything lower than some minimum, e.g. 1.
	 * That is the Minimum Limit.
	 */
	UE_API void SetMinimumLimit(const TOptional<double>& InMinimum);
	UFUNCTION(BlueprintCallable, Category = "GameSetting")
	UE_API void SetMinimumLimitValue(bool bInHasMinimum, double InMinimum);

	/**
	 * The SetSourceRangeAndStep defines the actual range the numbers could move in, but rarely
	 * the true maximum for the user is less than the maximum source range, so for example, the range
	 * of some slider might be 0..100, but you want to restrict the slider so that while it shows
	 * a bar that travels from 0 to 100, the user can't set anything lower than some maximum, e.g. 95.
	 * That is the Maximum Limit.
	 */
	UE_API void SetMaximumLimit(const TOptional<double>& InMaximum);
	UFUNCTION(BlueprintCallable, Category = "GameSetting")
	UE_API void SetMaximumLimitValue(bool bInHasMaximum, double InMaximum);
	
protected:
	/** UGameSettingValue */
	UE_API virtual void OnInitialized() override;

	UE_API void OnDataSourcesReady();

protected:

	TSharedPtr<FGameSettingDataSource> Getter;
	TSharedPtr<FGameSettingDataSource> Setter;

	TOptional<double> DefaultValue;
	double InitialValue = 0;

	TRange<double> SourceRange = TRange<double>(0, 1);
	double SourceStep = 0.01;
	TOptional<double> Minimum;
	TOptional<double> Maximum;

	FSettingScalarFormatFunction DisplayFormat;
};

#undef UE_API
