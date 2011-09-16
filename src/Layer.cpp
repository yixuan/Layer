#include "LayerStruct.h"
#include "LayerDesc.h"
#include "LayerDevice.h"
#include "Layer.h"


SEXP do_layer_device(SEXP widthInInches, SEXP heightInInches, SEXP fontPath)
{
    LayerDevice layerDev = LayerDevice();
    layerDev.CreateDevice(REAL(widthInInches)[0], REAL(heightInInches)[0]);
    layerDev.ConfigureDevice();
    layerDev.ShowDeviceWindow();
    layerDev.AddDeviceToR();

    set_default_font(fontPath);

    return R_NilValue;
}

SEXP set_default_font(SEXP fontPath)
{
    pGEDevDesc curGE = GEcurrentDevice();
    LayerDesc *ld = (LayerDesc *) curGE->dev->deviceSpecific;

    ld->SetDefaultFontFace(CHAR(STRING_ELT(fontPath, 0)));

    return R_NilValue;
}

SEXP get_layers()
{
    pGEDevDesc curGE = GEcurrentDevice();
    LayerDesc *ld = (LayerDesc *) curGE->dev->deviceSpecific;

    SEXP layerIDs, layerNames, result;
    PROTECT(layerIDs = NEW_INTEGER(ld->GetLayersCount()));
    PROTECT(layerNames = NEW_CHARACTER(ld->GetLayersCount()));
    PROTECT(result = NEW_LIST(2));
    gint i;
    for(i = 0; i < ld->GetLayersCount(); i++)
    {
        INTEGER_POINTER(layerIDs)[i] = ld->GetLayerIdAt(i);
        SET_STRING_ELT(layerNames, i, Rf_mkChar(ld->GetLayerNameAt(i)));
    }
    SET_VECTOR_ELT(result, 0, layerIDs);
    SET_VECTOR_ELT(result, 1, layerNames);
    UNPROTECT(3);
    return result;
}

SEXP get_id_count()
{
    pGEDevDesc curGE = GEcurrentDevice();
    LayerDesc *ld = (LayerDesc *) curGE->dev->deviceSpecific;

    SEXP idCount;
    PROTECT(idCount = NEW_INTEGER(1));
    INTEGER_POINTER(idCount)[0] = ld->GetIdCount();
    UNPROTECT(1);
    return idCount;
}

SEXP add_layer(SEXP layerName)
{
    pGEDevDesc curGE = GEcurrentDevice();
    LayerDesc *ld = (LayerDesc *) curGE->dev->deviceSpecific;

    SEXP id;
    PROTECT(id = NEW_INTEGER(1));
    INTEGER_POINTER(id)[0] = ld->AddLayer(CHAR(STRING_ELT(layerName, 0)));
    UNPROTECT(1);
    return id;
}

SEXP get_current_layer()
{
    pGEDevDesc curGE = GEcurrentDevice();
    LayerDesc *ld = (LayerDesc *) curGE->dev->deviceSpecific;

    gint currentIndex = ld->GetCurrentLayerIndex();
    SEXP id, layerName, result;
    PROTECT(id = NEW_INTEGER(1));
    PROTECT(layerName = NEW_CHARACTER(1));
    PROTECT(result = NEW_LIST(2));
    INTEGER_POINTER(id)[0] =ld->GetLayerIdAt(currentIndex);
    SET_STRING_ELT(layerName, 0, Rf_mkChar(ld->GetLayerNameAt(currentIndex)));
    SET_VECTOR_ELT(result, 0, id);
    SET_VECTOR_ELT(result, 1, layerName);
    UNPROTECT(3);
    return result;
}

SEXP set_current_layer_by_index(SEXP index)
{
    pGEDevDesc curGE = GEcurrentDevice();
    LayerDesc *ld = (LayerDesc *) curGE->dev->deviceSpecific;

    ld->SetCurrentLayerByIndex(INTEGER(index)[0]);
    return R_NilValue;
}
