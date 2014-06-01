#ifndef _OSMAND_CORE_I_MAP_KEYED_SYMBOLS_PROVIDER_H_
#define _OSMAND_CORE_I_MAP_KEYED_SYMBOLS_PROVIDER_H_

#include <OsmAndCore/stdlib_common.h>
#include <functional>

#include <OsmAndCore/QtExtensions.h>
#include <QList>

#include <OsmAndCore.h>
#include <OsmAndCore/CommonTypes.h>
#include <OsmAndCore/Map/MapSymbol.h>
#include <OsmAndCore/Map/IMapKeyedDataProvider.h>

namespace OsmAnd
{
    class OSMAND_CORE_API IMapKeyedSymbolsProvider : public IMapKeyedDataProvider
    {
        Q_DISABLE_COPY(IMapKeyedSymbolsProvider);
    private:
    protected:
        IMapKeyedSymbolsProvider();
    public:
        virtual ~IMapKeyedSymbolsProvider();
    };

    class OSMAND_CORE_API KeyedSymbolsData : public MapKeyedData
    {
        Q_DISABLE_COPY(KeyedSymbolsData);

    private:
    protected:
    public:
        KeyedSymbolsData(const std::shared_ptr<MapSymbolsGroup>& group, const Key key);
        virtual ~KeyedSymbolsData();

        const std::shared_ptr<MapSymbolsGroup> group;
    };
}

#endif // !defined(_OSMAND_CORE_I_MAP_KEYED_SYMBOLS_PROVIDER_H_)