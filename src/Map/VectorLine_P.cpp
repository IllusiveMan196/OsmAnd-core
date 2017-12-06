#include "VectorLine_P.h"
#include "VectorLine.h"
#include "Utilities.h"

#include "ignore_warnings_on_external_includes.h"
#include "restore_internal_warnings.h"

#include "MapSymbol.h"
#include "MapSymbolsGroup.h"
#include "VectorMapSymbol.h"
#include "OnSurfaceVectorMapSymbol.h"
#include "QKeyValueIterator.h"

OsmAnd::VectorLine_P::VectorLine_P(VectorLine* const owner_)
    : owner(owner_)
{
}

OsmAnd::VectorLine_P::~VectorLine_P()
{
}

bool OsmAnd::VectorLine_P::isHidden() const
{
    QReadLocker scopedLocker(&_lock);

    return _isHidden;
}

void OsmAnd::VectorLine_P::setIsHidden(const bool hidden)
{
    QWriteLocker scopedLocker(&_lock);

    _isHidden = hidden;
    _hasUnappliedChanges = true;
}

QVector<OsmAnd::PointI> OsmAnd::VectorLine_P::getPoints() const
{
    QReadLocker scopedLocker(&_lock);

    return _points;
}

void OsmAnd::VectorLine_P::setPoints(const QVector<PointI>& points)
{
    QWriteLocker scopedLocker(&_lock);

    _points = points;
    _hasUnappliedChanges = true;
}

bool OsmAnd::VectorLine_P::hasUnappliedChanges() const
{
    QReadLocker scopedLocker(&_lock);

    return _hasUnappliedChanges;
}

bool OsmAnd::VectorLine_P::applyChanges()
{
    QReadLocker scopedLocker1(&_lock);
    
    if (!_hasUnappliedChanges)
        return false;

    QReadLocker scopedLocker2(&_symbolsGroupsRegistryLock);
    for (const auto& symbolGroup_ : constOf(_symbolsGroupsRegistry))
    {
        const auto symbolGroup = symbolGroup_.lock();
        if (!symbolGroup)
            continue;

        for (const auto& symbol_ : constOf(symbolGroup->symbols))
        {
            symbol_->isHidden = _isHidden;

            if (const auto symbol = std::dynamic_pointer_cast<OnSurfaceVectorLineMapSymbol>(symbol_))
            {
                symbol->setPoints31(_points);
            }
        }
    }

    _hasUnappliedChanges = false;

    return true;
}

std::shared_ptr<OsmAnd::VectorLine::SymbolsGroup> OsmAnd::VectorLine_P::inflateSymbolsGroup() const
{
    QReadLocker scopedLocker(&_lock);

    // Construct new map symbols group for this marker
    const std::shared_ptr<VectorLine::SymbolsGroup> symbolsGroup(new VectorLine::SymbolsGroup(
        std::const_pointer_cast<VectorLine_P>(shared_from_this())));
    symbolsGroup->presentationMode |= MapSymbolsGroup::PresentationModeFlag::ShowAllOrNothing;

    int order = owner->baseOrder;
    if (!_points.empty())
    {
        const std::shared_ptr<OnSurfaceVectorLineMapSymbol> vectorLine(new OnSurfaceVectorLineMapSymbol(
            symbolsGroup));
        
        vectorLine->order = order++;
        vectorLine->setPoints31(_points);
        //VectorMapSymbol::generateLinePrimitive(*vectorLine, _points, owner->lineWidth, owner->fillColor);
        
        
        vectorLine->releaseVerticesAndIndices();
        int pointsCount = _points.size();
        vectorLine->primitiveType = VectorMapSymbol::PrimitiveType::TriangleStrip;
        
        // Line has no reusable vertices - TODO clarify
        vectorLine->indices = nullptr;
        vectorLine->indicesCount = 0;
        
        // Allocate space for vertices
        
        
        vectorLine->scaleType = VectorMapSymbol::ScaleType::InMeters;
        vectorLine->scale = 1;
        vectorLine->direction = 0;
        
        
        vectorLine->verticesCount = 0;
        if (pointsCount >= 2) {
            vectorLine->verticesCount = pointsCount * 2;
            vectorLine->vertices = new VectorMapSymbol::Vertex[vectorLine->verticesCount];
            auto pVertex = vectorLine->vertices;
            double nx = owner->lineWidth;
            double ny = 0;
            double ntan = atan2(_points[1].x - _points[0].x, _points[1].y - _points[0].y);
            nx = owner->lineWidth * sin(ntan);
            ny = owner->lineWidth * cos(ntan);
            
            for (auto pointIdx = 0u; pointIdx < pointsCount; pointIdx++)
            {
                
                int prevIndex = 0;
                double distX = Utilities::distance(
                    Utilities::convert31ToLatLon(PointI(_points[pointIdx].x, _points[prevIndex].y)),
                    Utilities::convert31ToLatLon(PointI(_points[prevIndex].x, _points[prevIndex].y)));
                
                double distY = Utilities::distance(
                    Utilities::convert31ToLatLon(PointI(_points[prevIndex].x, _points[pointIdx].y)),
                    Utilities::convert31ToLatLon(PointI(_points[prevIndex].x, _points[prevIndex].y)));
                
                if(_points[prevIndex].x > _points[pointIdx].x) {
                    distX = -distX;
                }
                if(_points[prevIndex].y > _points[pointIdx].y) {
                    distY = -distY;
                }
                pVertex->positionXY[0] = distX - nx;
                pVertex->positionXY[1] = distY - ny;
                pVertex->color = owner->fillColor;
                pVertex += 1;
                
                pVertex->positionXY[0] = distX + nx;
                pVertex->positionXY[1] = distY + ny;
                pVertex->color = owner->fillColor;
                pVertex += 1;
                
                if(pointIdx > 0 && pointIdx < pointsCount - 1) {
                    ntan = atan2(_points[pointIdx - 1].x - _points[pointIdx].x, _points[pointIdx - 1].y - _points[pointIdx].y);
                    nx = owner->lineWidth * sin(ntan);
                    ny = owner->lineWidth * cos(ntan);
                    pVertex->positionXY[0] = distX + nx;
                    pVertex->positionXY[1] = distY + ny;
                    pVertex->color = owner->fillColor;
                    pVertex += 1;
                }
            }
            
        }
        vectorLine->isHidden = _isHidden;
        symbolsGroup->symbols.push_back(vectorLine);
    }

    return symbolsGroup;
}

std::shared_ptr<OsmAnd::VectorLine::SymbolsGroup> OsmAnd::VectorLine_P::createSymbolsGroup() const
{
    const auto inflatedSymbolsGroup = inflateSymbolsGroup();
    registerSymbolsGroup(inflatedSymbolsGroup);
    return inflatedSymbolsGroup;
}

void OsmAnd::VectorLine_P::registerSymbolsGroup(const std::shared_ptr<MapSymbolsGroup>& symbolsGroup) const
{
    QWriteLocker scopedLocker(&_symbolsGroupsRegistryLock);

    _symbolsGroupsRegistry.insert(symbolsGroup.get(), symbolsGroup);
}

void OsmAnd::VectorLine_P::unregisterSymbolsGroup(MapSymbolsGroup* const symbolsGroup) const
{
    QWriteLocker scopedLocker(&_symbolsGroupsRegistryLock);

    _symbolsGroupsRegistry.remove(symbolsGroup);
}

