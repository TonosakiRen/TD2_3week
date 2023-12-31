Texture2D<float4> tex : register(t1);
SamplerState smp : register(s0);

struct ViewProjection {
	float32_t4x4 view;
	float32_t4x4 projection;
	float32_t3 viewPosition;
};
ConstantBuffer<ViewProjection> gViewProjection  : register(b0);

struct Material {
	float32_t4 materialColor; //Materialの色
	float32_t4x4 uvTransfrom;//uvtransform
	int32_t enableLighting; //lighitngするか
};
ConstantBuffer<Material> gMaterial  : register(b1);

struct DirectionLight {
	float32_t4 color;     //ライトの色
	float32_t3 direction; //ライト向き
	float32_t intensity;   //輝度
};
ConstantBuffer<DirectionLight> gDirectionLight  : register(b2);

struct VSOutput {
	float32_t4 svpos : SV_POSITION;
	float32_t3 normal : NORMAL;
	float32_t2 uv : TEXCOORD;
	float32_t3 worldPosition : POSITION;
};

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VSOutput input) {


	float32_t3 normal = normalize(input.normal);

	PixelShaderOutput output;
	output.color = float32_t4(0.0f, 0.0f, 0.0f, 1.0f);

	// マテリアル
	float32_t4 tranformedUV = mul(float32_t4(input.uv, 0.0f, 1.0f), gMaterial.uvTransfrom);
	float32_t4 texColor = tex.Sample(smp, tranformedUV.xy);
	output.color.xyz += gMaterial.materialColor.xyz * texColor.xyz;

	if (gMaterial.enableLighting != 0) {
		// 陰
		/*float32_t NdotL = dot(normal, -normalize(gDirectionLight.direction));
		float32_t cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
		output.color.xyz *= gDirectionLight.color.xyz * cos * gDirectionLight.intensity;

		float32_t3 viewDirection = normalize(gViewProjection.viewPosition - input.worldPosition);*/

		// フレネル
		//float32_t3 fresnelColor = float32_t3(1.0f, 0.0f, 0.0f);
		//float32_t power = 2.0f;
		//float32_t fresnel = pow((1.0f - saturate(dot(normal, viewDirection))), power);
		////output.color.xyz = lerp(1.0f - fresnel, output.color.xyz, fresnelColor);
		//output.color.xyz += fresnelColor * fresnel;

		//反射
		/*float32_t3 reflectVec = reflect(gDirectionLight.direction, normal);
		float32_t3 speculerColor = float32_t3(1.0f, 1.0f, 1.0f);
		float32_t specluerPower = 100.0f;
		output.color.xyz += speculerColor * pow(saturate(dot(reflectVec, viewDirection)), specluerPower);*/
	}

	return output;
}