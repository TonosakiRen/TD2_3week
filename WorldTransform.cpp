#include "WorldTransform.h"
#include <cassert>
#include "externals/DirectXTex/d3dx12.h"
#include "DirectXCommon.h"

using namespace DirectX;

void WorldTransform::Initialize() {
	CreateConstBuffer();
	Map();
	UpdateMatrix();
}

void WorldTransform::CreateConstBuffer() {
    HRESULT result;
    ID3D12Device* device = DirectXCommon::GetInstance()->GetDevice();
    // ヒーププロパティ
    CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    // リソース設定
    CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataWorldTransform) + 0xff) & ~0xff);

    // 定数バッファの生成
    result = device->CreateCommittedResource(
        &heapProps, // アップロード可能
        D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
        IID_PPV_ARGS(&constBuff_));
    assert(SUCCEEDED(result));
}

void WorldTransform::Map() {
    // 定数バッファとのデータリンク
    HRESULT result = constBuff_->Map(0, nullptr, (void**)&constMap);
    assert(SUCCEEDED(result));
}

void WorldTransform::UpdateMatrix() {
    Matrix4x4 matScale, matRot, matTrans;

    // スケール、回転、平行移動行列の計算
    matScale = MakeScaleMatrix(scale_);

    matRot = MakeIdentity4x4();
    matRot *= MakeRotateZMatrix(rotation_.z);
    matRot *= MakeRotateXMatrix(rotation_.x);
    matRot *= MakeRotateYMatrix(rotation_.y);
    matTrans = MakeTranslateMatrix(translation_);

    // ワールド行列の合成
    matWorld_ = MakeIdentity4x4(); 
    matWorld_ *= matScale;          
    matWorld_ *= matRot;            
    matWorld_ *= matTrans;         


    // 親行列の指定がある場合は、掛け算する
    if (parent_) {
        //scaleを反映させない
        Matrix4x4 inverseMatrix;
        if (!isScaleParent_) {
            inverseMatrix = Inverse(MakeScaleMatrix(MakeScale(parent_->matWorld_)));
            matWorld_ *= inverseMatrix;
        }

        matWorld_ *= parent_->matWorld_;
    }

    // 定数バッファに書き込み
    if (constMap) {
        constMap->matWorld = matWorld_;
    }
}