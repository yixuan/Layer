\name{Layer}
\alias{Layer}
\title{Open a Layer device}
\usage{
Layer(width = 7, height = 7, fontPath = NULL)
}
\arguments{
  \item{width}{the width of the device in inches}

  \item{height}{the height of the device in inches}

  \item{fontPath}{the path of the default .ttf font file.
  If \code{NULL}, a built-in font file will be used.}
}
\description{
  Open a graphics device with layer support.
}
\note{
  Currently the device could not be resized after
  initialization.
}
\author{
  Yixuan Qiu (\email{yixuan.qiu@cos.name})
}
