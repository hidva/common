#define NDEBUG
#include <stdio.h>
#include <string.h>

#include <string>
#include <boost/random.hpp>

#include "folly/FBString.h"
#include "execute_time_get/execute_time_get.h"
#include "log/pp_qq_log.h"

static const int seed = 1;
typedef boost::mt19937 RandomT;
static RandomT rng(seed);

/* 生成位于 [low,up]范围的随机数.
 */
template <class Integral1, class Integral2>
Integral2 random(Integral1 low, Integral2 up) {
  boost::uniform_int<> range(low, up);
  return range(rng);
}

const std::string kSrcStr = "\
Even if you've mastered the art of the cover letter and the resume, \
another part of the job search process can trip up an otherwise \
qualified candidate: the writing sample.\n\
\n\
Strong writing and communication skills are highly sought after by \
most employers. Whether crafting short emails or lengthy annual \
reports, many workers use their writing skills every day. And for an \
employer seeking proof behind that ubiquitous candidate \
phrase,\"excellent communication skills\", a required writing sample \
is invaluable.\n\
\n\
Writing samples need the same care and attention given to cover \
letters and resumes. Candidates with otherwise impeccable credentials \
are routinely eliminated by a poorly chosen writing sample. Notice I \
said \"poorly chosen\" not \"poorly written.\" Because that's the rub: \
a writing sample not only reveals the individual's writing skills, it \
also offers a peek into what they consider important or relevant for \
the position. If you miss that mark with your writing sample, don't \
expect to get a call for an interview.";

const std::string kDstStr = "So how do you tackle that writing sample request?";

const folly::fbstring kSrcFBStr = kSrcStr;
const folly::fbstring kDstFBStr = kDstStr;

const size_t kSrcSize = kSrcStr.size();
const size_t kDstSize = kDstStr.size();


#if 1
int
main(int ,char **)
{
    PP_QQ_LOG_D("hello");

    // find successful test
    {
        auto FBStringFind = [&] {
            size_t pos = random<size_t>(0,kSrcSize);
            size_t len = random<size_t>(0,kSrcSize - pos);
            kSrcFBStr.find(kSrcFBStr.data(),pos,len);
        };
        
        auto MemMemFind = [&] {
            size_t pos = random<size_t>(0,kSrcSize);
            size_t len = random<size_t>(0,kSrcSize - pos);
            memmem(kSrcFBStr.data() + pos,kSrcSize - pos,kSrcFBStr.data(),len);
        };
        
        auto StringFind = [&] {
            size_t pos = random<size_t>(0,kSrcSize);
            size_t len = random<size_t>(0,kSrcSize - pos);
            kSrcStr.find(kSrcFBStr.data(),pos,len);
        };
        
        PP_QQ_LOG_D("FBStringFind: %lu",ExecuteTimeGet(33,1000000,FBStringFind));
        PP_QQ_LOG_D("MemMemFind: %lu",ExecuteTimeGet(33,1000000,MemMemFind));
        PP_QQ_LOG_D("StringFind: %lu",ExecuteTimeGet(33,1000000,StringFind));
    }
    
    // find unsuccessful
    {
        auto FBStringFind = [&] {
            kSrcFBStr.find(kDstFBStr.data(),0,kDstFBStr.size());
        };
        
        auto StringFind = [&] {
            kSrcStr.find(kDstFBStr.data(),0,kDstFBStr.size());
        };
        
        auto MemMemFind = [&] {
            memmem(kSrcFBStr.data(),kSrcSize,kDstFBStr.data(),kDstSize);
        };

        PP_QQ_LOG_D("FBStringFind: %lu",ExecuteTimeGet(33,1000000,FBStringFind));
        PP_QQ_LOG_D("MemMemFind: %lu",ExecuteTimeGet(33,1000000,MemMemFind));
        PP_QQ_LOG_D("StringFind: %lu",ExecuteTimeGet(33,1000000,StringFind));
    }
    
    return 0;
}

#else


int
main(int ,char **)
{
    PP_QQ_LOG_D("hello");
    PP_QQ_LOG_D("%zu",random<size_t,size_t>(0,1332));
    PP_QQ_LOG_D("%zu",random<size_t,size_t>(0,1332));
    PP_QQ_LOG_D("%zu",random<size_t,size_t>(0,1332));
    PP_QQ_LOG_D("%zu",random<size_t,size_t>(0,1332));
    return 0;
}

#endif