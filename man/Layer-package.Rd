\docType{package}
\name{Layer-package}
\alias{Layer-package}
\title{R Graphics Device with Layer Support}
\description{
  This package implements an \R graphics device that allows
  users to create graphics based on layers.
}
\details{
  Most graphics devices in \R allow users to draw shapes on
  a single canvas. That is to say, after executing plotting
  commands, the curves and shapes previously drawn are
  mixed up and could not be seperated with each other. As a
  result, these patterns could not be moved, scaled or
  rotated by consequent \R commands.

  The Layer device provided by this package has the
  capacity to create images made up of several layers. A
  layer can be thought of a transparent canvas with opaque
  patterns on it. The image we see on screen is the
  combination of all layers, but each of them is
  independent of the others. Therefore, we can apply
  operations(e.g. moving, scaling or rotation) to a
  specific layer without changing the patterns on other
  layers.

  When a layer is created, it will be assigned an ID number
  and a name. ID is an integer to uniquely identify a
  layer, and it is created automatically by the device. The
  name is just like a note of the content of the layer, so
  it can be any character string.

  In Layer device, plotting commands can only be passed to
  one layer at a time, which we use the term "Current
  Layer" to stand for. Users can change Current Layer by
  the \code{\link{set.current.layer}()} function. To see
  the information of available layers, use the
  \code{\link{get.layers}()} and
  \code{\link{get.current.layer}()} functions.

  Note that high-level plotting functions will destroy all
  previously created layers and open a new one, so if you
  want to create a multi-layer graph, be sure to use
  low-level plotting functions like \code{points()} and
  \code{lines()}.

  Layer device also provides an interactive operation with
  the image on the screen. You can adjust the location of
  the Current Layer by pressing and moving your mouse. To
  do this, press your left mouse button and keep it down,
  and then move Current layer along with your mouse, and
  finally relocate the layer by releasing your mouse
  button. The operations are similar if you want to scale
  or rotate a layer.
}
\author{
  Yixuan Qiu (\email{yixuan.qiu@cos.name})
}
\keyword{device}
\keyword{iplot}
