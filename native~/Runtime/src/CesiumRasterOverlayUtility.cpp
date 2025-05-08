#include "CesiumRasterOverlayUtility.h"

#include <CesiumAsync/IAssetResponse.h>
#include <CesiumRasterOverlays/RasterOverlay.h>
#include <CesiumGeospatial/GlobeRectangle.h>

#include <DotNet/CesiumForUnity/CesiumRasterOverlay.h>
#include <DotNet/CesiumForUnity/CesiumRasterOverlayLoadFailureDetails.h>
#include <DotNet/System/String.h>
#include <DotNet/Unity/Mathematics/double2.h>
#include <DotNet/System/Collections/Generic/List1.h>


using namespace DotNet;
using namespace CesiumRasterOverlays;

namespace CesiumForUnityNative {
/*static*/ RasterOverlayOptions CesiumRasterOverlayUtility::GetOverlayOptions(
    const DotNet::CesiumForUnity::CesiumRasterOverlay& overlay) {
  RasterOverlayOptions options{};
  options.maximumScreenSpaceError = overlay.maximumScreenSpaceError();
  options.maximumSimultaneousTileLoads = overlay.maximumSimultaneousTileLoads();
  options.screenSpaceErrorDistancePer = overlay.screenSpaceErrorDistancePer();

  System::Collections::Generic::List1<::DotNet::Unity::Mathematics::double2> filterRectangeLeftBottom = overlay.filterRectangeLeftBottom();
  System::Collections::Generic::List1<::DotNet::Unity::Mathematics::double2> filterRectangeRightUp = overlay.filterRectangeRightUp();
  int32_t len = filterRectangeLeftBottom.Count();
  if (len > filterRectangeRightUp.Count())
  {
    len = filterRectangeRightUp.Count();
  }

  for (int32_t i = 0; i < len; ++i) {
    const ::DotNet::Unity::Mathematics::double2 rectangeLeftBottom = filterRectangeLeftBottom[i];
    const ::DotNet::Unity::Mathematics::double2 rectangeRightUp = filterRectangeRightUp[i];
    CesiumGeospatial::GlobeRectangle globalRectangle(
        rectangeLeftBottom.x,
        rectangeLeftBottom.y,
        rectangeRightUp.x,
        rectangeRightUp.y);
    options.filterRectange.push_back(globalRectangle);
  }
  
  options.maximumTextureSize = overlay.maximumTextureSize();
  options.subTileCacheBytes = overlay.subTileCacheBytes();
  options.showCreditsOnScreen = overlay.showCreditsOnScreen();
  options.loadErrorCallback =
      [overlay](const RasterOverlayLoadFailureDetails& details) {
        int typeValue = (int)details.type;
        long statusCode = details.pRequest && details.pRequest->response()
                              ? details.pRequest->response()->statusCode()
                              : 0;
        CesiumForUnity::CesiumRasterOverlayLoadFailureDetails unityDetails(
            overlay,
            CesiumForUnity::CesiumRasterOverlayLoadType(typeValue),
            statusCode,
            System::String(details.message));

        CesiumForUnity::CesiumRasterOverlay::
            BroadcastCesiumRasterOverlayLoadFailure(unityDetails);
      };

  return options;
}
} // namespace CesiumForUnityNative
