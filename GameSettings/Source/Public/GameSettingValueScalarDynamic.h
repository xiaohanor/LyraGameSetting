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
enum class EGameSettingScalarFormatPreset : uint8
{
	Raw,
	RawOneDecimal,
	RawTwoDecimals,
	ZeroToOnePercent,
	ZeroToOnePercent_OneDecimal,
	SourceAsPercent1,
	SourceAsPercent100,
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

	UFUNCTION(BlueprintCallable, Category="GameSetting|Dynamic")
	UE_API void SetDynamicGetterPath(const TArray<FString>& InGetterPath);

	UFUNCTION(BlueprintCallable, Category="GameSetting|Dynamic")
	UE_API void SetDynamicSetterPath(const TArray<FString>& InSetterPath);

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
	UFUNCTION(BlueprintCallable, Category="GameSetting|Dynamic")
	UE_API void SetDefaultValue(double InValue);

	/**  */
	UE_API void SetDisplayFormat(FSettingScalarFormatFunction InDisplayFormat);
	UFUNCTION(BlueprintCallable, Category="GameSetting|Dynamic")
	UE_API void SetDisplayFormatPreset(EGameSettingScalarFormatPreset InPreset);
	
	/**  */
	UE_API void SetSourceRangeAndStep(const TRange<double>& InRange, double InSourceStep);
	UFUNCTION(BlueprintCallable, Category="GameSetting|Dynamic")
	UE_API void SetSourceRangeAndStepValues(double InMinValue, double InMaxValue, double InSourceStep);
	
	/**
	 * The SetSourceRangeAndStep defines the actual range the numbers could move in, but often
	 * the true minimum for the user is greater than the minimum source range, so for example, the range
	 * of some slider might be 0..100, but you want to restrict the slider so that while it shows 
	 * a bar that travels from 0 to 100, the user can't set anything lower than some minimum, e.g. 1.
	 * That is the Minimum Limit.
	 */
	UE_API void SetMinimumLimit(const TOptional<double>& InMinimum);
	UFUNCTION(BlueprintCallable, Category="GameSetting|Dynamic")
	UE_API void SetMinimumLimitValue(double InMinimum);
	UFUNCTION(BlueprintCallable, Category="GameSetting|Dynamic")
	UE_API void ClearMinimumLimit();

	/**
	 * The SetSourceRangeAndStep defines the actual range the numbers could move in, but rarely
	 * the true maximum for the user is less than the maximum source range, so for example, the range
	 * of some slider might be 0..100, but you want to restrict the slider so that while it shows
	 * a bar that travels from 0 to 100, the user can't set anything lower than some maximum, e.g. 95.
	 * That is the Maximum Limit.
	 */
	UE_API void SetMaximumLimit(const TOptional<double>& InMaximum);
	UFUNCTION(BlueprintCallable, Category="GameSetting|Dynamic")
	UE_API void SetMaximumLimitValue(double InMaximum);
	UFUNCTION(BlueprintCallable, Category="GameSetting|Dynamic")
	UE_API void ClearMaximumLimit();
	
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
