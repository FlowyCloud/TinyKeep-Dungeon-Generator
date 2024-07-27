
#include "DynamicTexture.h"

#include "RHICommandList.h"
#include "Rendering/Texture2DResource.h"

FDynamicTexture::FDynamicTexture()
{
}

FDynamicTexture::FDynamicTexture(FIntPoint InTextureSize, FLinearColor InBackgroundColor)
{
	TextureSize = InTextureSize;
	BackgroundColor = InBackgroundColor;

	InitializeTexture();
}

void FDynamicTexture::FillTexture(FLinearColor Color) const
{
	for (int i = 0; i < TextureTotalPixels; i++)
	{
		TextureData[i * 4] = Color.B * 255;
		TextureData[i * 4 + 1] = Color.G * 255;
		TextureData[i * 4 + 2] = Color.R * 255;
		TextureData[i * 4 + 3] = Color.A * 255;
	}
}

void FDynamicTexture::SetPixelColor(int X, int Y, FLinearColor Color) const
{
	// If Pixel is outside of Texture return
	if (X < 0 || Y < 0 || X >= TextureSize.X || Y >= TextureSize.Y) {
		return;
	}

	// Get the Start of the Pixel Data
	int start = ((Y * TextureSize.X) + X) * 4;

	// Set Pixel Value by Offsetting from the Start of the Pixel Data
	TextureData[start] = Color.B * 255;
	TextureData[start + 1] = Color.G * 255;
	TextureData[start + 2] = Color.R * 255;
	TextureData[start + 3] = Color.A * 255;
}

void FDynamicTexture::SetTextureFromData(const TMap<FIntPoint, FLinearColor>& TextureMap) const
{
	for (const TPair<FIntPoint, FLinearColor> MapPair : TextureMap)
	{
		SetPixelValue(MapPair.Key.X, MapPair.Key.Y, MapPair.Value.ToFColor(true));
	}
}

void FDynamicTexture::SetPixelValue(int X, int Y, FColor Color) const
{
	// If Pixel is outside of Texture return
	if (X < 0 || Y < 0 || X >= TextureSize.X || Y >= TextureSize.Y) {
		return;
	}

	// Get the Start of the Pixel Data
	int start = ((Y * TextureSize.X) + X) * 4;

	// Set Pixel Value by Offsetting from the Start of the Pixel Data
	TextureData[start] = Color.B;
	TextureData[start + 1] = Color.G;
	TextureData[start + 2] = Color.R;
	TextureData[start + 3] = Color.A;
}

void FDynamicTexture::DrawRectangle(int StartX, int StartY, int Width, int Height, FLinearColor Color)
{
	for (int y = 0; y < Height; y++)
	{
		for (int x = 0; x < Width; x++) {
			SetPixelColor(StartX + x, StartY + y, Color);
		}
	}

	UpdateTexture();
}

void FDynamicTexture::DrawCircle(int StartX, int StartY, int Size, FLinearColor Color, bool Center)
{
	float radius = Size / 2;
	int offset = FMath::Floor(radius * Center);

	for (int y = 0; y < Size; y++)
	{
		for (int x = 0; x < Size; x++) {
			FVector pos = FVector(x - radius, y - radius, 0);
			if (pos.Size2D() <= radius) {
				SetPixelColor((StartX - offset) + x, (StartY - offset) + y, Color);
			}
		}
	}

	UpdateTexture();
}

void FDynamicTexture::DrawFromTexture(int StartX, int StartY, UTexture2D* Texture, FLinearColor Filter /*= FLinearColor::White*/)
{
	if (!Texture) {
		return;
	}

	int width = Texture->GetSizeX();
	int height = Texture->GetSizeY();
	int texDataSize = width * height * 4;
	uint8* texData = new uint8[texDataSize];

	FTexture2DMipMap& readMip = Texture->GetPlatformData()->Mips[0];
	readMip.BulkData.GetCopy((void**)&texData);


	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++) {
			int start = ((y * width) + x) * 4;

			SetPixelValue(StartX + x, StartY + y, FColor((texData[start + 2] * Filter.R), texData[start + 1] * Filter.G, texData[start] * Filter.B, texData[start + 3] * Filter.A));
		}
	}

	FMemory::Free(texData);
}

void FDynamicTexture::InitializeTexture()
{
	// Get Total Pixels in Texture
	TextureTotalPixels = TextureSize.X * TextureSize.Y;

	// Get Total Bytes of Texture - Each pixel has 4 bytes for RGBA
	TextureDataSize = TextureTotalPixels * 4;
	TextureDataSqrtSize = TextureSize.X * 4;

	// Initialize Texture Data Array
	TextureData = new uint8[TextureDataSize];

	// Create Dynamic Texture Object
	DynamicTexture = UTexture2D::CreateTransient(TextureSize.X, TextureSize.Y);
	DynamicTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
	DynamicTexture->SRGB = 0;
	DynamicTexture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
	DynamicTexture->Filter = TextureFilter::TF_Nearest;
	DynamicTexture->AddToRoot();
	DynamicTexture->UpdateResource();

	//Create Update Region Struct Instance
	TextureRegion = new FUpdateTextureRegion2D(0, 0, 0, 0, TextureSize.X, TextureSize.Y);

	FillTexture(BackgroundColor);
	UpdateTexture();
}

void FDynamicTexture::UpdateTexture(bool bFreeData)
{
	if (DynamicTexture == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Dynamic Texture tried to Update Texture but it hasn't been initialized!"));
		return;
	}

	struct FUpdateTextureRegionsData
	{
		FTexture2DResource* Texture2DResource;
		FRHITexture2D* TextureRHI;
		int MipIndex;
		int NumRegions;
		FUpdateTextureRegion2D* Regions;
		int SrcPitch;
		int SrcBpp;
		uint8* SrcData;
	};

	FUpdateTextureRegionsData* RegionData = new FUpdateTextureRegionsData;

	UTexture2D* Texture = DynamicTexture;

	RegionData->Texture2DResource = (FTexture2DResource*)Texture->GetResource();
	RegionData->TextureRHI = RegionData->Texture2DResource->GetTexture2DRHI();
	RegionData->MipIndex = 0;
	RegionData->NumRegions = 1;
	RegionData->Regions = TextureRegion;
	RegionData->SrcPitch = TextureDataSqrtSize;
	RegionData->SrcBpp = 4;
	RegionData->SrcData = TextureData;

	ENQUEUE_RENDER_COMMAND(UpdateTextureRegionsData)(
		[RegionData, bFreeData, Texture](FRHICommandListImmediate& RHICmdList)
		{
			for (int RegionIndex = 0; RegionIndex < RegionData->NumRegions; ++RegionIndex)
			{
				int CurrentFirstMip = Texture->FirstResourceMemMip;
				if (RegionData->TextureRHI && RegionData->MipIndex >= CurrentFirstMip)
				{
					RHIUpdateTexture2D(
						RegionData->TextureRHI,
						RegionData->MipIndex - CurrentFirstMip,
						RegionData->Regions[RegionIndex],
						RegionData->SrcPitch,
						RegionData->SrcData
						+ RegionData->Regions[RegionIndex].SrcY * RegionData->SrcPitch
						+ RegionData->Regions[RegionIndex].SrcX * RegionData->SrcBpp
					);
				}
			}
			if (bFreeData) {
				FMemory::Free(RegionData->Regions);
				FMemory::Free(RegionData->SrcData);
			}
			delete RegionData;
});
}

UTexture* FDynamicTexture::GetTexture()
{
	UpdateTexture();
	return DynamicTexture;
}
