#ifndef ORG_PP_QQ_COMMON_CIPHER_CTR_CIPHER_MODE_H
#define ORG_PP_QQ_COMMON_CIPHER_CTR_CIPHER_MODE_H

#include "cipher/counter.h"
#include "cipher/block_cipher.h"

struct CTRCipherMode : public BlockCipherMode {

    inline void SetCounterImpl(pp_qq::Counter *impl) noexcept;
    inline pp_qq::Counter* GetCounterImpl() const noexcept;

public:
    bool IsNeedPadding() const noexcept override;
    void OnCipherBegin(BlockCipher *block_cipher) override;
    void Update(BlockCipher *block_cipher,void *dst,const void *src,size_t size) override;
    void OnCipherEnd(BlockCipher *block_cipher) override;

private:

    pp_qq::Counter *counter_impl_ = nullptr;

    /* 这里可以用 ExtendedStdString next_counter_;然后在 OnCipherBegin() 时分配空间.
     *
     * 但是这里考虑到块加密的共性,大声的告诉我,有哪个块加密的块长会超过 32 bytes么~
     * 如果有,我就把 kBlockSizeMax 改为 64 好了
     */
    enum {
        kBlockSizeMax = 32
    };
    unsigned char next_counter_[kBlockSizeMax];
};

void CTRCipherMode::SetCounterImpl(pp_qq::Counter *impl) noexcept
{
    counter_impl_ = impl;
    return ;
}

pp_qq::Counter* CTRCipherMode::GetCounterImpl() const noexcept
{
    return counter_impl_;
}



#endif // ORG_PP_QQ_COMMON_CIPHER_CTR_CIPHER_MODE_H
