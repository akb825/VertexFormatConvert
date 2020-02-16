# Tool

The vfc tool provides a command-line interface for performing vertex format conversions, making it easy to integrate into scripts.

# Command-line options
- `-h, --help`: Prints the help message and exits.
- `-i, --input <file>`: Path to a JSON file that defines the input to process. If not provided, input will be read from stdin.
- `-o, --output <dir>`: Path to a directory to output the results to. The directory will be created if it doesn't exist.

# Input

The primary input is in the form of a JSON configuration. The file has the following structure, with members required unless otherwise stated:

- `vertexFormat`: The vertex format to convert to. It is an array of objects with the following members:
	- `name`: The name of the element.
	- `layout`: The data layout of the element (described below).
	- `type`: The data type of the element (described below).
- `indexType`: (optional) The type of the index to output to (described below). If not provided or null, no indices will be produced.
- `primitiveType`: (optional) The type of the primitive (described below). If not provided, TriangleList will be assumed.
- `patchPoints`: (required for PatchList primitive type) The number of patch points when the primitive type is PatchList.
- `vertexStreams`: The input vertex streams to read data from. It is an array of objects with the following members:
	- `vertexFormat`: The vertex format of the vertex stream. See the above vertexFormat layout description for details.
	- `vertexData`: The path to a data file containing the vertex data.
	- `indexType`: (optional) The type of the input index data. If not provided or null, index data isn't used.
	- `indexData`: (required if indexType is set) The path to a data file containing the index data.
- `vertexTransforms`: (optional) The transforms to apply to vertex data on conversion. It is an array of objects with the following members:
	- `name`: The name of the element.
	- `transform`: The transform to apply (described below).

## General notes on input

- Names for enums (e.g. layout, type) are case-insensitive. However, names provided by 'name' elements are case sensitive when matching with each-other.
- File paths may either be absolute or relative to the input json file. When with stdin, the current working directory is used for relative paths.
- Data files are binary files that contain the raw data as described by the vertex format for index type. The size is expected to match exactly the vertex or index type multiplied by the number of elements.

## Supported vertex layouts

- X8
- X8Y8
- X8Y8Z8
- X8Y8Z8W8
- W2X10Y10Z10
- W2Z10Y10X10
- X16
- X16Y16
- X16Y16Z16
- X16Y16Z16W16
- X32
- X32Y32
- X32Y32Z32
- X32Y32Z32W32
- X64
- X64Y64
- X64Y64Z64
- X64Y64Z64W64
- Z10Y11X11_UFloat
- E5Z9Y9X9_UFloat

Note: RGBA may also be used in place of XYZW.

## Supported vertex types

- UNorm
- SNorm
- UInt
- SInt
- Float

## Supported index types

- UInt16
- UInt32

## Supported primitive types

- PointList
- LineList
- LineStrip
- TriangleList
- TriangleStrip
- TriangleFan
- PatchList

# Output

The general output is printed to stdout as JSON with the following layout:

- `vertexFormat`: The verex format that was output. It is an array of objects with the following members:
	- `name`: The name of the element.
	- `layout`: The data layout of the element.
	- `type`: The data type of the element.
	- `offset`: The offset in bytes from the start of the vertex to the element.
	- `minValue`: The minimum vertex value for this element as 4-element array.
	- `maxValue`: The maximum vertex value for this element as 4-element array.
- `vertexStride`: The size in bytes of each vertex.
- `vertexCount`: The number of vertices that were output.
- `vertexData`: The path to a data file containing the output vertices.
- `indexType`: (set if indexType was set on input) The type of the index data.
- `indexBuffers`: (set if indexType was set on input) The index buffers that were output. It is an array of objects with the following elements:
	- `indexCount`: The number of indices for this buffer.
	- `baseVertex`: The value to add to each index value to get the final vertex index. This can be applied when drawing the mesh.
	- `indexData`: The path to a data file containing the output indices.

All output files are placed in the directory provided by the --output command-line option.
