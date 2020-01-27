# Library

The VFC library is a C++ library that provides utilities for converting between various vertex formats for runtime use, including interleaving separate vertex streams and combinging separate indices into a single index set.

# Vertex formats

Vertex formats are represented with the `VertexFormat` class. Each element has three components:

* A human-readable name.
* The layout, represented with the `ElementLayout` enum. The enum names are the component name (X, Y, Z, W, with aliases for R, G, B, A) followed by the number of bits. For example, `ElementLayout::X16Y16` contains the X and Y components with 16 bits each.
* The type, represented with the `ElementType` enum.

The `VertexFormat::appendElement()` function is used to add elements to the `VertexFormat` instance. It is expected that each element has a unique name and that the layout/type combination is valid. (i.e. `isElementValid(layout, type)` returns `true`) The main restrictions are:

* 8-bit 2-10-10-10 bit elements cannot be floats.
* 32 and 64-bit elements cannot be normalized types.
* `ElementLayout::Z10Y11X11_UFloat` and `ElementLayout::E5Z9Y9X9_UFloat` elements must be floats.

`VertexFormat` may be accessed like a vector to access each `VertexElement` added to the format. In addition to the name, layout, and type, each `VertexElement` contains the byte offset into the combined vertex structure. The total size of each vertex in bytes can be accessed with `VertexFormat::stride()`.

# Vertex values

The `VertexValue` class holds the value for a single vertex element represented as four doubles. This is a generic storage for the data, which can be decoded from data with `VertexValue::fromData()`, or encoded to data with `VertexValue::toData()`.

In order to support packing vertices into compact forms, `VertexValue::toData()` an optional bounding box. When used with a normalized type, the values will be converted to a normalized value (\[0, 1\] for `ElementType::UNorm`, \[-1, 1\] for `ElementType::SNorm`) to span the minimum and maximum values for the box.

> **Note:** since `VertexValue` stores the intermediate values as doubles, it technically cannot represent the full range of 64-bit integer values. Supporting this would be possible, but would be significantly more complicated to implement. This isn't expected to come up in typical usage, so this is currently considered an acceptable limitation.

# Converter

The `Converter` class provides a high-level interface to perform vertex conversions typical for 3D model conversions. Input model data is commonly provided as multiple vertex streams with independent indices, which the `Converter` class can convert to a single combined vertex stream with optional indices.

When constructing a `Converter`, the `VertexFormat` and `IndexType` are provided for the output format. The index type may be `IndexType::NoIndices` if no indices are desired. `PrimitiveType` is also provided to determine what kind of primitives are used, which will alter behavior when the maximum index value is used. If `PrimitiveType::PatchList` is used, the number of points for each patch is provided.

An error handler may optionally be provided to `Converter` during construction. This allows custom handling for message, otherwise errors will be output to `stderr` by default. Errors during construction can be detected by calling `Converter::isValid()` or converting the `Converter` instance to a `bool`.

After a `Converter` has been constructed, one or more vertex streams can be added with `Converter::addVertexStream()`. Each vertex stream has its own `VertexFormat` and the data associated with it, and may optionally contain indices. The `VertexFormat` may contain multiple elements. Vertex elements are matched by name with the `VertexFormat` provided during construction, and extra vertex elements are ignored. Index data may optionally be provided to `Converter::addVertexStream()`, otherwise the vertex values are read in sequence.

Some things to keep in mind when adding vertex streams:

* The number of indices (or non-indexed vertices) must match between streams.
* Primitive restart index values are supported for strip and fan primitive types, but require indices to be output from the converter.
* Vertex elements defined in the `VertexFormat` provided during construction may only be defined by one vertex stream.
* All vertex elements defined in the `VertexFormat` provided during construction must be defined by vertex streams before `Converter::convert()` is called.

Before conversion, transforms may be set for vertex elements by calling `Converter::setElementTransform()`, either for the index of the element in the `VertexFormat` provided during constructor or element name. Options are:

* `Transform::Identity`: doesn't modify the value during conversion. This is the default.
* `Transform::Bounds`: when used with a normalized vertex type, normalizeds the values (\[0, 1\] for `ElementType::UNorm`, \[-1, 1\] for `ElementType::SNorm`) to span the minimum and maximum values for the vertex values. The original value can be extracted by interpolating between the minimum and maximum values (queried by Converter::getVertexElementBounds()) with the normalized value.
* `Transform::UNormToSNorm`: converts from a value in the range \[0, 1\] to the range \[-1, 1\].
* `Transform::SNormToUNorm`: converts from a value in the range \[-1, 1\] to the range \[0, 1\].

Once everything has been set up, call `Converter::convert()` to perform the conversion. This will do the following:

* Convert the vertex values according to the `VertexFormat` provided during construction, applying the transform set for each element.
* When indices are used, vertices with identical values (with a byte-level comparison) will share the same index value.
* When the maximum index value is exceeded, a separate index buffer is supported. This is represented with the `IndexData` struct, which contains a `baseVertex` to add to each index value. This is can be provided to draw calls to most modern graphics APIs, otherwise it can be used to offset the vertex buffer during binding.

After conversion, the vertex data can be queried with `Converter::getVertices()` and index data with `Converter::getIndices()`.

## Example

```
// Positions as 2-component float values.
float positions[] =
{
	-1.0f, -1.0f,
	 1.0f, -1.0f,
	-1.0f,  1.0f,
	 1.0f,  1.0f
};

// Indices for positions.
std::uint32_t positionIndices[] = {0, 1, 2, 2, 1, 3};

// Vertex format for position input stream.
vfc::VertexFormat positionFormat;
positionFormat.appendElement("positions", vfc::ElementLayout::X32Y32, vfc::ElementType::Float);

// Texture coordinates as 2-component float values.
float texCoords[] =
{
	1.0f, 1.0f,
	0.0f, 1.0f,
	1.0f, 0.0f,
	0.0f, 0.0f
};

// Separate indices for texture coordinates. These can be different values and even a different type
std::uint16_t texCoordIndices[] = {3, 2, 1, 1, 2, 0};

// Vertex format for texture coordinate input stream.
vfc::VertexFormat texCoordFormat;
texCoordFormat.appendElement("texCoords", vfc::ElementLayout::X32Y32, vfc::ElementType::Float);

// Vertex format to convert to.
vfc::VertexFormat vertexFormat;
// Convert positions to 16-bit floats.
vertexFormat.appendElement("positions", vfc::ElementLayout::X16Y16, vfc::ElementType::Float);
// Convert texture coordinates to normalized 16-bit values.
vertexFormat.appendElement("texCoords", vfc::ElementLayout::X16Y16, vfc::ElementType::UNorm);

// Construct converter with the vertex format, index type, and primitive type to convert to.
vfc::Converter converter(vertexFormat, vfc::IndexType::UInt16, vfc::PrimitiveType::TriangleList);

// Add the vertex streams for the positions and texture coordinates.
converter.addVertexStream(std::move(positionFormat), positions, 4,
	vfc::IndexType::UInt32, positionIndices, 6);
converter.addVertexStream(std::move(texCoordFormat), texCoords, 4,
	vfc::IndexType::UInt32, texCoordIndices, 6);

// Perform the conversion.
converter.convert();

// Output indices. If there were enough vertices to exceed the maximum index value indices will
// contain multiple elements, in this case it should have a size of 1.
const std::vector<vfc::IndexData>& indices = converter.getIndices();

// Converted vertices.
const std::vector<std::uint8_t>& vertices = converter.getVertices();
```
