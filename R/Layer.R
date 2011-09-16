##' Open a Layer device
##'
##' Open a graphics device with layer support.
##'
##' @param width the width of the device in inches
##' @param height the height of the device in inches
##' @param fontPath the path of the default .ttf font file. If \code{NULL},
##' a built-in font file will be used.
##' @note Currently the device could not be resized after initialization.
##' @author Yixuan Qiu (\email{yixuan.qiu@@cos.name})
##' @export
Layer <- function(width = 7, height = 7, fontPath = NULL) {
	if(is.null(fontPath)) fontPath = system.file("font", "WenQuanYiMicroHei.ttf",
												 package = "Layer")
	.Call("do_layer_device", as.numeric(width), as.numeric(height),
		  as.character(fontPath), PACKAGE = "Layer")
	invisible(NULL)
}

##' Get layer information
##'
##' This function returns the IDs and names of all available layers on
##' the active Layer device. If no Layer device is active, an
##' error occurs.
##'
##' In Layer device every layer has been assigned an ID number and a name.
##' ID is an integer to uniquely identify a layer, and it is created
##' automatically by the device. The name is just like a note of the
##' content of the layer, so it can be any character string.
##' 
##' @return A data frame listing the ID and name of all available layers.
##' @author Yixuan Qiu (\email{yixuan.qiu@@cos.name})
##' @export
get.layers <- function() {
	if(.Device != "Layer") stop("Current device is not a \"Layer\" device!")
	result = .Call("get_layers", PACKAGE = "Layer")
	layers = data.frame(ID = result[[1]], Name = result[[2]])
	return(layers)
}

##' Add a new layer
##'
##' This function adds a new layer to the current active Layer device.
##' If no Layer device is active, an error occurs.
##'
##' The newly created layer will be set as the "Current Layer". It means
##' that consequent low-level plotting functions will be passed to
##' this layer. (High-level plotting functions will destroy all layers and
##' create a new one.)
##'
##' @param layerName the name of the newly added layer. If \code{NULL},
##'                  it will be assigned automatically.
##' @return A data frame listing the ID and name of the newly added layer.
##' @author Yixuan Qiu (\email{yixuan.qiu@@cos.name})
##' @export
add.layer <- function(layerName = NULL) {
	if(.Device != "Layer") stop("Current device is not a \"Layer\" device!")
	if(is.null(layerName))
	{
		layerName = sprintf("Layer_%d", .Call("get_id_count", PACKAGE = "Layer"))
	}
	layerName = as.character(layerName)
	id = .Call("add_layer", layerName, PACKAGE = "Layer")
	layer = data.frame(ID = id, Name = layerName)
	invisible(layer)
}

##' Set a layer as the Current Layer
##'
##' This function sets a layer given by its ID as the Current Layer.
##'
##' @param id the unique ID number of the layer.
##' @return A data frame listing the ID and name of this layer.
##' @author Yixuan Qiu (\email{yixuan.qiu@@cos.name})
##' @export
set.current.layer <- function(id) {
	if(.Device != "Layer") stop("Current device is not a \"Layer\" device!")
	ind = which(get.layers()$ID == id)
	if(!length(ind)) stop("Layer not found!")
	invisible(set.current.layer.by.ind(ind))
}

set.current.layer.by.ind <- function(index = 1) {
	if(.Device != "Layer") stop("Current device is not a \"Layer\" device!")
	result = .Call("set_current_layer_by_index", as.integer(index - 1), PACKAGE = "Layer")
	layer = get.current.layer()
	invisible(layer)
}

##' Get the ID and name of the Current Layer
##'
##' This function gets the ID and name of the Current Layer.
##'
##' @return A data frame listing the ID and name of the Current Layer.
##' @author Yixuan Qiu (\email{yixuan.qiu@@cos.name})
##' @export
get.current.layer <- function() {
	if(.Device != "Layer") stop("Current device is not a \"Layer\" device!")
	result = .Call("get_current_layer", PACKAGE = "Layer")
	layer = data.frame(ID = result[[1]], Name = result[[2]])
	return(layer)
}

.onLoad <- function(lib, pkg) {
	library.dynam("Layer", pkg, lib)
}

