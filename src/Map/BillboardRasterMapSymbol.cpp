#include "BillboardRasterMapSymbol.h"

OsmAnd::BillboardRasterMapSymbol::BillboardRasterMapSymbol(
    const std::shared_ptr<MapSymbolsGroup>& group_)
    : RasterMapSymbol(group_)
    , drawAlongPath(false)
    , elevation(NAN)
    , elevationScaleFactor(1.0f)
{
}

OsmAnd::BillboardRasterMapSymbol::~BillboardRasterMapSymbol()
{
}

OsmAnd::PointI OsmAnd::BillboardRasterMapSymbol::getOffset() const
{
    return offset;
}

void OsmAnd::BillboardRasterMapSymbol::setOffset(const PointI newOffset)
{
    offset = newOffset;
}

OsmAnd::PointI OsmAnd::BillboardRasterMapSymbol::getPosition31() const
{
    return position31;
}

void OsmAnd::BillboardRasterMapSymbol::setPosition31(const PointI position)
{
    position31 = position;
}

float OsmAnd::BillboardRasterMapSymbol::getElevation() const
{
    return elevation;
}

void OsmAnd::BillboardRasterMapSymbol::setElevation(const float newElevation)
{
    elevation = newElevation;
}

float OsmAnd::BillboardRasterMapSymbol::getElevationScaleFactor() const
{
    return elevationScaleFactor;
}

void OsmAnd::BillboardRasterMapSymbol::setElevationScaleFactor(const float scaleFactor)
{
    elevationScaleFactor = scaleFactor;
}
