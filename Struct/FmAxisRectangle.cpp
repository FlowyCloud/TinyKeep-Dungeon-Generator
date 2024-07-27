
#include "FmAxisRectangle.h"

FmAxisRectangle::FmAxisRectangle()
{
}

FmAxisRectangle::FmAxisRectangle(FVector2D MinPara, FVector2D MaxPara)
{
	minX = MinPara.X;
	maxX = MaxPara.X;
	minY = MinPara.Y;
	maxY = MaxPara.Y;
	Area = (maxX - minX) * (maxY - minY);
	CircumRadius = (FMath::Sqrt(FMath::Square(maxX - minX) + FMath::Square(maxY - minY))) / 2;
	Vertices = { FVector2D(MinPara.X, MinPara.Y), FVector2D(MaxPara.X, MinPara.Y), FVector2D(MaxPara.X, MaxPara.Y), FVector2D(MinPara.X, MaxPara.Y) };
	Edges = { FmEdge(Vertices[0], Vertices[1]), FmEdge(Vertices[1], Vertices[2]), FmEdge(Vertices[2], Vertices[3]), FmEdge(Vertices[3], Vertices[0]) };
	Center = FVector2D((minX + maxX) / 2, (minY + maxY) / 2);
}


bool FmAxisRectangle::HasInside(FVector2D InVector)
{
	return (InVector.X > minX && InVector.X < maxX && InVector.Y > minY && InVector.Y < maxY);
}

bool FmAxisRectangle::IsOverlappedWith(const FmAxisRectangle& RectanglePara)
{
	if ( (RectanglePara.maxX > minX) && (RectanglePara.minX < maxX) && (RectanglePara.maxY > minY) && (RectanglePara.minY < maxY) )
		return true;
	else
		return false;
}

void FmAxisRectangle::DrawRectangle(UWorld* World, FColor InColor, float InThickness, float InZ)
{
	DrawDebugLine(World, FVector(Vertices[0].X, Vertices[0].Y, InZ), FVector(Vertices[1].X, Vertices[1].Y, InZ), InColor, true, -1, 0, InThickness);
	DrawDebugLine(World, FVector(Vertices[1].X, Vertices[1].Y, InZ), FVector(Vertices[2].X, Vertices[2].Y, InZ), InColor, true, -1, 0, InThickness);
	DrawDebugLine(World, FVector(Vertices[2].X, Vertices[2].Y, InZ), FVector(Vertices[3].X, Vertices[3].Y, InZ), InColor, true, -1, 0, InThickness);
	DrawDebugLine(World, FVector(Vertices[3].X, Vertices[3].Y, InZ), FVector(Vertices[0].X, Vertices[0].Y, InZ), InColor, true, -1, 0, InThickness);
}

