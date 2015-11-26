cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VertexInputType
{
	float4 position: POSITION;
	float4 colour: COLOR;
};

struct PixelInputType
{
	float4 position:SV_POSITION;
	float4 colour:COLOR;
};

PixelInputType main(VertexInputType input)
{
	PixelInputType output;

	// change the position vector into a 4 unit vector
	input.position.w = 1.0f;

	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.colour = input.colour;

	return output;
}