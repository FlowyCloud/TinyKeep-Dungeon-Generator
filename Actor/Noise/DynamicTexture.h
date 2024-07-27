#pragma once

#include "DynamicTexture.generated.h"

USTRUCT(BlueprintType)
struct FDynamicTexture
{
	GENERATED_BODY()
	
	FDynamicTexture();
	FDynamicTexture(FIntPoint InTextureSize, FLinearColor InBackgroundColor);

	// VARIABLE
	FIntPoint TextureSize = FIntPoint(512, 512);
	FLinearColor BackgroundColor = FLinearColor::White;
	
	// Array that contains the Texture Data
	uint8* TextureData;

	// Total Bytes of Texture Data
	int TextureDataSize = 0;

	// Texture Data Sqrt Size
	int TextureDataSqrtSize = 0;

	// Total Count of Pixels in Texture
	int TextureTotalPixels = 0;

	// Texture Object
	UTexture2D* DynamicTexture = nullptr;

	// Update Region Struct
	FUpdateTextureRegion2D* TextureRegion = nullptr;

	// FUNCTION

	/// Fill Entire Texture with a specified color.
	void FillTexture(FLinearColor Color) const;
	void SetPixelColor(int32 X, int32 Y, FLinearColor Color) const;
	void SetTextureFromData(const TMap<FIntPoint, FLinearColor>& TextureMap) const;
	void SetPixelValue(int32 X, int32 Y, FColor Color) const;
	void DrawRectangle(int32 StartX, int32 StartY, int32 Width, int32 Height, FLinearColor Color);
	void DrawCircle(int32 StartX, int32 StartY, int32 Size, FLinearColor Color, bool Center = true);
	void DrawFromTexture(int32 StartX, int32 StartY, UTexture2D* Texture, FLinearColor Filter = FLinearColor::White);
	
	// Initialize the Dynamic Texture
	void InitializeTexture();
	// Update Texture Object from Texture Data
	void UpdateTexture(bool bFreeData = false);
	UTexture* GetTexture();
	
};
