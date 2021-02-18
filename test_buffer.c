#include <assert.h>
#include "buffer.h"

int main(void) {
    FileManager *fm;
    LogManager *lm;
    Buffer *buff;

    // ディレクトリtmp_test_bufferと
    // データサイズが400バイトのブロックを定義
    fm = new_FileManager("tmp_test_buffer", 400);

    lm = new_LogManager(fm, "logfile");

    buff = new_Buffers(fm, lm, 1);
}
