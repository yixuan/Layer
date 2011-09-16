#include "LayerStruct.h"
#include "LayerDesc.h"


LayerStruct::LayerStruct(const gchar* lName, gint id, gdouble wInPoints, gdouble hInPoints, gint wInPixels, gint hInPixels)
{
    layerName = g_string_new(lName);
    ID = id;
#ifdef LAYER_DEBUG
Rprintf("LayerStruct Constructor - %s\n", layerName->str);
#endif
    widthInPoints = wInPoints;
    heightInPoints = hInPoints;
    widthInPixels = wInPixels;
    heightInPixels = hInPixels;

    dispSurface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, wInPixels, hInPixels);
    recordSurface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, wInPoints, hInPoints);
    dispCr = cairo_create(dispSurface);
    recordCr = cairo_create(recordSurface);

    funRecorderBuffer = g_ptr_array_sized_new(RecorderBufferSize);
    parRecorderBuffer = g_ptr_array_sized_new(RecorderBufferSize);
    funRecorder = g_ptr_array_sized_new(RecorderSize);
    parRecorder = g_ptr_array_sized_new(RecorderSize);

    scaleCenterX = 0.0;
    scaleCenterY = 0.0;
    rotCenterX = widthInPixels / 2.0;
    rotCenterY = heightInPixels / 2.0;
    scaleX = widthInPixels / widthInPoints;
    scaleY = heightInPixels / heightInPoints;
    rotAngle = 0.0;
    transX = transY = 0.0;
    this->SetDefaultTransMatrix();

    fpCrSave = &LayerStruct::CrSave;
    fpCrRestore = &LayerStruct::CrRestore;
    fpCrSetSourceRGBA = &LayerStruct::CrSetSourceRGBA;
    fpCrSetOperatorSource = &LayerStruct::CrSetOperatorSource;
    fpCrSetLineWidth = &LayerStruct::CrSetLineWidth;
    fpCrSetLineCap = &LayerStruct::CrSetLineCap;
    fpCrSetLineJoin = &LayerStruct::CrSetLineJoin;
    fpCrSetMiterLimit = &LayerStruct::CrSetMiterLimit;
    fpCrSetDash = &LayerStruct::CrSetDash;
    fpCrRotate = &LayerStruct::CrRotate;
    fpCrMoveTo = &LayerStruct::CrMoveTo;
    fpCrRelMoveTo = &LayerStruct::CrRelMoveTo;
    fpCrLineTo = &LayerStruct::CrLineTo;
    fpCrCircle = &LayerStruct::CrCircle;
    fpCrRectangle = &LayerStruct::CrRectangle;
    fpCrClip = &LayerStruct::CrClip;
    fpCrResetClip = &LayerStruct::CrResetClip;
    fpCrClosePath = &LayerStruct::CrClosePath;
    fpCrStroke = &LayerStruct::CrStroke;
    fpCrFillPreserve = &LayerStruct::CrFillPreserve;
    fpCrPaint = &LayerStruct::CrPaint;
    fpCrSelectFontFace = &LayerStruct::CrSelectFontFace;
    fpCrSetDefaultFontFace = &LayerStruct::CrSetDefaultFontFace;
    fpCrSetFontSize = &LayerStruct::CrSetFontSize;
    fpCrShowText = &LayerStruct::CrShowText;
}

LayerStruct::~LayerStruct()
{
#ifdef LAYER_DEBUG
Rprintf("LayerStruct Destructor - %s\n", layerName->str);
#endif
    if(recordCr) cairo_destroy(recordCr);
    if(recordSurface) cairo_surface_destroy(recordSurface);
    if(dispCr) cairo_destroy(dispCr);
    if(dispSurface) cairo_surface_destroy(dispSurface);
    if(layerName) g_string_free(layerName, FALSE);
    if(funRecorderBuffer)
    {
        g_ptr_array_free(funRecorderBuffer, TRUE);
    }
    if(parRecorderBuffer)
    {
        for(guint i = 0; i < parRecorderBuffer->len; i++)
        {
            g_free(g_ptr_array_index(parRecorderBuffer, i));
        }
        g_ptr_array_free(parRecorderBuffer, TRUE);
    }
    if(funRecorder)
    {
#ifdef LAYER_DEBUG
Rprintf("funRecorder length is %d\n", funRecorder->len);
#endif
        g_ptr_array_free(funRecorder, TRUE);
    }
    if(parRecorder)
    {
#ifdef LAYER_DEBUG
Rprintf("parRecorder length is %d\n", parRecorder->len);
#endif
        for(guint i = 0; i < parRecorder->len; i++)
        {
            g_free(g_ptr_array_index(parRecorder, i));
        }
        g_ptr_array_free(parRecorder, TRUE);
    }
}


void LayerStruct::SetFillColorFromR(pGEcontext gc, gboolean allowZeroAlpha)
{
    gdouble *pt = g_new0(gdouble, 4);
    pt[0] = R_RED(gc->fill) / 255.0;
    pt[1] = R_GREEN(gc->fill) / 255.0;
    pt[2] = R_BLUE(gc->fill) / 255.0;
    pt[3] = R_TRANSPARENT(gc->fill) ? (!allowZeroAlpha) : (R_ALPHA(gc->fill) / 255.0);
    this->CrSetSourceRGBA(pt);
}
void LayerStruct::SetLineColorFromR(pGEcontext gc)
{
    gdouble *pt = g_new0(gdouble, 4);
    pt[0] = R_RED(gc->col) / 255.0;
    pt[1] = R_GREEN(gc->col) / 255.0;
    pt[2] = R_BLUE(gc->col) / 255.0;
    pt[3] = R_ALPHA(gc->col) / 255.0;
    this->CrSetSourceRGBA(pt);
}
void LayerStruct::SetLineTypeFromR(pGEcontext gc)
{
    gdouble *wd = g_new0(gdouble, 1);
    //wd[0] = MAX(gc->lwd / 2.0, 1);
    wd[0] = gc->lwd / 2.0;
    this->CrSetLineWidth(wd);

    gint *cap = g_new0(gint, 1);
    switch(gc->lend)
    {
    case GE_ROUND_CAP:
        cap[0] = CAIRO_LINE_CAP_ROUND;
        break;
    case GE_BUTT_CAP:
        cap[0] = CAIRO_LINE_CAP_BUTT;
        break;
    case GE_SQUARE_CAP:
        cap[0] = CAIRO_LINE_CAP_SQUARE;
        break;
    }
    this->CrSetLineCap(cap);

    gint *join = g_new0(gint, 1);
    gdouble *miter = g_new0(gdouble, 1);
    switch(gc->ljoin)
    {
    case GE_ROUND_JOIN:
        join[0] = CAIRO_LINE_JOIN_ROUND;
        g_free(miter);
        break;
    case GE_MITRE_JOIN:
        join[0] = CAIRO_LINE_JOIN_MITER;
        miter[0] = gc->lmitre;
        this->CrSetMiterLimit(miter);
        break;
    case GE_BEVEL_JOIN:
        join[0] = CAIRO_LINE_JOIN_BEVEL;
        g_free(miter);
        break;
    }
    this->CrSetLineJoin(join);

    gdouble *dash = g_new0(gdouble, 10);
    dash[0] = 0;
    gint i;
    gint lty = gc->lty;
    for(i = 0; i < 8 && lty & 15; i++)
    {
        dash[i + 2] = lty & 15;
        lty = lty >> 4;
    }
    dash[1] = i;
    this->CrSetDash(dash);
}

void LayerStruct::Clear()
{
    // Clear all history
    if(funRecorderBuffer->len > 0)
    {
        g_ptr_array_free(funRecorderBuffer, TRUE);
        funRecorderBuffer = g_ptr_array_sized_new(RecorderBufferSize);
    }
    if(parRecorderBuffer->len > 0)
    {
        for(guint i = 0; i < parRecorderBuffer->len; i++)
        {
            g_free(g_ptr_array_index(parRecorderBuffer, i));
        }
        g_ptr_array_free(parRecorderBuffer, TRUE);
        parRecorderBuffer = g_ptr_array_sized_new(RecorderBufferSize);
    }
    if(funRecorder->len > 0)
    {
        g_ptr_array_free(funRecorder, TRUE);
        funRecorder = g_ptr_array_sized_new(RecorderSize);
    }
    if(parRecorder->len > 0)
    {
        for(guint i = 0; i < parRecorder->len; i++)
        {
            g_free(g_ptr_array_index(parRecorder, i));
        }
        g_ptr_array_free(parRecorder, TRUE);
        parRecorder = g_ptr_array_sized_new(RecorderSize);
    }
    cairo_save(dispCr);
    cairo_set_operator(dispCr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(dispCr);
    cairo_restore(dispCr);
    cairo_save(recordCr);
    cairo_set_operator(recordCr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(recordCr);
    cairo_restore(recordCr);
}

void LayerStruct::PaintWithBgColor(pGEcontext gc, gboolean allowZeroAlpha)
{
    this->Clear();
    this->CrSave(NULL);
    this->SetFillColorFromR(gc, allowZeroAlpha);
    this->CrSetOperatorSource(NULL);
    this->CrPaint(NULL);
    this->CrRestore(NULL);
}

void LayerStruct::ClipRectangle(gdouble x0, gdouble x1, gdouble y0, gdouble y1)
{
    gdouble *pt = g_new0(gdouble, 4);
    pt[0] = MIN(x0, x1);
    pt[1] = MIN(y0, y1);
    pt[2] = abs(x0 - x1);
    pt[3] = abs(y0 - y1);
    this->CrResetClip(NULL);
    this->CrRectangle(pt);
    this->CrClip(NULL);
}

void LayerStruct::PathCircle(gdouble x, gdouble y, gdouble r)
{
    gdouble *pt = g_new0(gdouble, 3);
    pt[0] = x;
    pt[1] = y;
    pt[2] = r;
    this->CrCircle(pt);
}

void LayerStruct::PathRect(gdouble x0, gdouble y0, gdouble x1, gdouble y1)
{
    gdouble *pt = g_new0(gdouble, 4);
    pt[0] = x0;
    pt[1] = y0;
    pt[2] = x1 - x0;
    pt[3] = y1 - y0;
    this->CrRectangle(pt);
}

void LayerStruct::PathLine(gdouble x1, gdouble y1, gdouble x2, gdouble y2)
{
    gdouble *pt1 = g_new0(gdouble, 2);
    gdouble *pt2 = g_new0(gdouble, 2);
    pt1[0] = x1;
    pt1[1] = y1;
    pt2[0] = x2;
    pt2[1] = y2;
    this->CrMoveTo(pt1);
    this->CrLineTo(pt2);
}

void LayerStruct::PathPolyline(gint n, gdouble *x, gdouble *y)
{
    gdouble *pt1 = g_new0(gdouble, 2);
    pt1[0] = x[0];
    pt1[1] = y[0];
    this->CrMoveTo(pt1);
    for(gint i = 1; i < n; i++)
    {
        gdouble *pt2 = g_new0(gdouble, 2);
        pt2[0] = x[i];
        pt2[1] = y[i];
        this->CrLineTo(pt2);
    }
}
void LayerStruct::PathPolygon(gint n, gdouble *x, gdouble *y)
{
    this->PathPolyline(n, x, y);
    this->CrClosePath(NULL);
}

void LayerStruct::Stroke(pGEcontext gc)
{
    this->SetLineColorFromR(gc);
    this->SetLineTypeFromR(gc);
    this->CrStroke(NULL);
}

void LayerStruct::StrokeAndFill(pGEcontext gc, gboolean allowZeroAlpha)
{
    if(gc->fill != NA_INTEGER)
    {
        this->SetFillColorFromR(gc, allowZeroAlpha);
        this->CrFillPreserve(NULL);
    }
    if(gc->col != NA_INTEGER)
    {
        this->Stroke(gc);
    }
}

void LayerStruct::SetFont(pGEcontext gc, pDevDesc dd)
{
    gint fontFace = gc->fontface;
    GString *fontFamily = g_string_new((const gchar*)gc->fontfamily);
    cairo_font_slant_t slant = CAIRO_FONT_SLANT_NORMAL;
    cairo_font_weight_t weight = CAIRO_FONT_WEIGHT_NORMAL;

    if(fontFace < 1 || fontFace > 5) fontFace = 1;
    if(fontFace == 5)
        g_string_assign(fontFamily, "symbol");
    else if(fontFamily->len != 0)
    {
        if(fontFace == 2 || fontFace == 4)
            weight = CAIRO_FONT_WEIGHT_BOLD;
        if(fontFace == 3 || fontFace == 4)
            slant = CAIRO_FONT_SLANT_ITALIC;
        gchar *pt1 = g_new0(gchar, fontFamily->len + 3);
        pt1[0] = (gchar)weight;
        pt1[1] = (gchar)slant;
        g_stpcpy(pt1 + 2, (const gchar *)fontFamily->str);
        this->CrSelectFontFace(pt1);
    } else {
        LayerDesc *ld = (LayerDesc *) dd->deviceSpecific;
        cairo_font_face_t **pt2 = g_new0(cairo_font_face_t *, 1);
        pt2[0] = ld->GetDefaultFontFace();
        this->CrSetDefaultFontFace(pt2);
    }
    gdouble *pt3 = g_new0(gdouble, 1);
    pt3[0] = gc->cex * gc->ps + 0.5;
    this->CrSetFontSize(pt3);
    g_string_free(fontFamily, TRUE);
}

gdouble LayerStruct::GetStrWidth(gchar *str_utf8, pGEcontext gc, pDevDesc dd)
{
    if(!str_utf8)    return 0.0;
    cairo_text_extents_t te;
    cairo_text_extents(dispCr, str_utf8, &te);
    g_free(str_utf8);
    return te.x_advance;
}

void LayerStruct::ShowText(gdouble x, gdouble y, gchar *str_utf8, gdouble rot, gdouble hadj,
                           pGEcontext gc, pDevDesc dd)
{
    this->CrSave(NULL);
    this->SetFont(gc, dd);
    cairo_text_extents_t te;
    cairo_text_extents(dispCr, str_utf8, &te);
    this->SetLineColorFromR(gc);
    gdouble *pt1 = g_new0(gdouble, 2);
    pt1[0] = x;
    pt1[1] = y;
    this->CrMoveTo(pt1);
    gdouble *pt2 = g_new0(gdouble, 1);
    pt2[0] = -rot / 180.0 * M_PI;
    this->CrRotate(pt2);
    gdouble *pt3 = g_new0(gdouble, 1);
    pt3[0] = -te.x_advance * hadj;
    this->CrRelMoveTo(pt3);
    this->CrShowText(str_utf8);
	this->CrRestore(NULL);
}

void LayerStruct::SetDefaultTransMatrix()
{
    /* Scale -> Rotate -> Move */
    gdouble cosa = cos(rotAngle), sina = sin(rotAngle);
    transMat.xx = scaleX * cosa,
    transMat.yx = scaleX * sina,
    transMat.xy = -scaleY * sina,
    transMat.yy = scaleY * cosa,
    transMat.x0 = cosa * (1 - scaleX) * scaleCenterX + (1 - cosa) * rotCenterX -
                  sina * (1 - scaleY) * scaleCenterY + sina * rotCenterY + transX,
    transMat.y0 = sina * (1 - scaleX) * scaleCenterX - sina * rotCenterX +
                  cosa * (1 - scaleY) * scaleCenterY + (1 - cosa) * rotCenterY + transY;
}

void LayerStruct::Scale(gdouble scX, gdouble scY)
{
    cairo_matrix_t mt;
    mt.xx = scX;
    mt.yy = scY;
    mt.xy = mt.yx = 0;
    mt.x0 = (1 - scX) * scaleCenterX;
    mt.y0 = (1 - scY) * scaleCenterY;
    cairo_matrix_t tmp = transMat;
    cairo_matrix_multiply(&transMat, &tmp, &mt);
}

void LayerStruct::Rotate(gdouble rAngle)
{
    cairo_matrix_t mt;
    gdouble cosa = cos(rAngle), sina = sin(rAngle);
    mt.xx = cosa;
    mt.xy = -sina;
    mt.yx = sina;
    mt.yy = cosa;
    mt.x0 = (1 - cosa) * rotCenterX + sina * rotCenterY;
    mt.y0 = (1 - cosa) * rotCenterY - sina * rotCenterX;
    cairo_matrix_t tmp = transMat;
    cairo_matrix_multiply(&transMat, &tmp, &mt);
}

void LayerStruct::Rotate(gdouble cosAngle, gdouble sinAngle)
{
    cairo_matrix_t mt;
    mt.xx = cosAngle;
    mt.xy = -sinAngle;
    mt.yx = sinAngle;
    mt.yy = cosAngle;
    mt.x0 = (1 - cosAngle) * rotCenterX + sinAngle * rotCenterY;
    mt.y0 = (1 - cosAngle) * rotCenterY - sinAngle * rotCenterX;
    cairo_matrix_t tmp = transMat;
    cairo_matrix_multiply(&transMat, &tmp, &mt);
}

void LayerStruct::Move(gdouble trX, gdouble trY)
{
    cairo_matrix_t mt;
    mt.xx = mt.yy = 1;
    mt.xy = mt.yx = 0;
    mt.x0 = trX;
    mt.y0 = trY;
    cairo_matrix_t tmp = transMat;
    cairo_matrix_multiply(&transMat, &tmp, &mt);
}


void LayerStruct::MergeDispSurface(LayerStruct *layer)
{
    cairo_save(dispCr);
    cairo_set_operator(dispCr, CAIRO_OPERATOR_OVER);
    cairo_set_source_surface(dispCr, layer->GetDispSurface(), 0.0, 0.0);
    cairo_paint(dispCr);
    cairo_restore(dispCr);
}

void LayerStruct::MergeDispSurfaceWithTransformation(LayerStruct *layer, cairo_matrix_t *mt)
{
    cairo_save(dispCr);
    cairo_set_operator(dispCr, CAIRO_OPERATOR_OVER);
    cairo_set_matrix(dispCr, mt);
    cairo_set_source_surface(dispCr, layer->GetRecordSurface(), 0.0, 0.0);
    cairo_paint(dispCr);
    cairo_restore(dispCr);
}




void LayerStruct::PlotToDispSurface()
{
    if(funRecorderBuffer->len)
    {
        cairo_save(dispCr);
        cairo_set_matrix(dispCr, &this->transMat);
        for(guint i = 0; i < funRecorderBuffer->len; i++)
        {
            LSMemFunPtr fun = *(LSMemFunPtr *)g_ptr_array_index(funRecorderBuffer, i);
            void *pt = g_ptr_array_index(parRecorderBuffer, i);
            (this->*fun)(pt, TRUE);

            cairo_t *tmp = dispCr;
            dispCr = recordCr;
            (this->*fun)(pt, TRUE);
            dispCr = tmp;

            g_ptr_array_add(funRecorder, g_ptr_array_index(funRecorderBuffer, i));
            g_ptr_array_add(parRecorder, g_ptr_array_index(parRecorderBuffer, i));
        }
        cairo_restore(dispCr);

        g_ptr_array_free(funRecorderBuffer, TRUE);
        funRecorderBuffer = g_ptr_array_sized_new(RecorderBufferSize);
        g_ptr_array_free(parRecorderBuffer, TRUE);
        parRecorderBuffer = g_ptr_array_sized_new(RecorderBufferSize);
    }
}

void LayerStruct::ReplotToDispSurface()
{
    if(funRecorder->len)
    {
        cairo_save(dispCr);
        cairo_set_operator(dispCr, CAIRO_OPERATOR_CLEAR);
        cairo_paint(dispCr);
        cairo_restore(dispCr);
        cairo_save(dispCr);
        cairo_set_matrix(dispCr, &this->transMat);
        for(guint i = 0; i < funRecorder->len; i++)
        {
            LSMemFunPtr fun = *(LSMemFunPtr *)g_ptr_array_index(funRecorder, i);
            void *pt = g_ptr_array_index(parRecorder, i);
            (this->*fun)(pt, TRUE);
        }
        cairo_restore(dispCr);
    }
}

void LayerStruct::ReplotTo(cairo_t *cr, gboolean isVectorDevice)
{
    if(funRecorder->len)
    {
        cairo_save(cr);
        cairo_matrix_t trans = this->transMat;
        if(isVectorDevice)
        {
            cairo_matrix_t mt;
            mt.xx = widthInPoints / widthInPixels;
            mt.yy = heightInPoints / heightInPixels;
            mt.xy = mt.yx = mt.x0 = mt.y0 = 0;
            cairo_matrix_multiply(&trans, &this->transMat, &mt);
        }
        cairo_set_matrix(cr, &trans);
        cairo_t *tmp = dispCr;
        dispCr = cr;
        for(guint i = 0; i < funRecorder->len; i++)
        {
            LSMemFunPtr fun = *(LSMemFunPtr *)g_ptr_array_index(funRecorder, i);
            void *pt = g_ptr_array_index(parRecorder, i);
            (this->*fun)(pt, TRUE);
        }
        dispCr = tmp;
        cairo_restore(cr);
    }
}
