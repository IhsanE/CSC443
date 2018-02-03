#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include "library.h"
using namespace std;


int main () {
    Record r = Record();
    char test_str[] = "ZNVLWXYIND,DWSNUDQCLJ,BJPOSHTXRT,JJRGIIQAGH,HWIRJDCYCA,HEKQTSQCOB,APVVVSAOUM,LQEWEXBXEU,XQYSBEDKYH,KNNLRMVKQW,DLVVPEZWQK,EQWPQGMIRK,EJFJQCBANY,EKVYAKNZXF,YEUCQQYWJK,XWGHEFLYCF,AJAELCZEQF,EFAWPOWFLX,WRHKFFVQWE,SHUXDQQVAN,XWIZXSRARY,XRTWUVWCWF,FBKVTDRDFC,YEYVYBXRFF,YCKCJNUVGR,PNXDREJCFS,OHEVKILSPP,GNLRRODWUT,FJBSEALDWT,WJZKYLYQFC,QKLRAPZBKV,HOXFUEMUTC,MYEYQSUNBS,EBAOAPKHTI,ULDTNTORJP,CRZSDDNCOH,IUASCXIWMB,UBNZGTNDEE,NARSCFRESA,CTCWBLNVLD,ZNOBKUXDDI,VHAEYMNSNA,HJJCDOYPYR,JFFEKWUPRM,BTSISQYFMO,ZOOCTABZMY,BIHBEOSDDP,GDLEAHXXLX,SLGUGXKASJ,XNXUDUBMFV,EEWAQLRBYE,LVAMZYMBCS,LXUERUAWPL,ZSWJIXPLCA,EDZBEJEYXF,LQWOFYCTEC,WDYISDDBJJ,ZXJPWPLHAQ,FSVFEIFYIG,HBVITATLNV,UQJHUMWXFN,YQSILWBRWS,XPVZMDIWMR,GUPJRSVLRM,TOLNIEZMJY,CJMAFPKGKW,NGOIODWYPE,CQAJDLHKWK,OBIRBZIHIZ,TLCBKXMCCK,NNSTEWEGIK,UKKHTVFSMQ,SEHJQSJQWF,MWBXTZLLLY,BZETOUHCOO,AGKBPRVDHH,ZKKGDVAEKG,TOSHQGJLPI,AGFYIDAFYV,SYGPNAKQJH,YEDPDEBCDD,PDUVJLRATB,FJVEVDHZZP,DBHXTSLEPD,FWNYTGRTDK,EUGEGSGFQH,REETYBGSTU,IGNIHJGOMZ,IQSQNLKMRY,CNQTPPUIKS,PFITHHRNFC,OKOZPDGLSO,JKFQWVBLAS,QCGQRQQYJT,DEBTILMMGB,IBNUSOPJZK,ICBTXKKUIX,FWIWSZOKYE,MSYMDHENKL,HTTXZBCEVZ";
    char* char_array = strtok(test_str, ",");
    while (char_array != NULL) {
        r.push_back(char_array);
        char_array = strtok(NULL, ",");
    }
    cout << "Original Record:\n" << endl;
    for (Record::iterator iter = (r).begin(); iter != (r).end(); iter++) {
        cout << r.end() - iter;
        cout << *iter << " ";
    }
    int size = fixed_len_sizeof(&r);
    cout << "\n\nSize of Record\n";
    cout << size << endl;

    void* buffer = malloc(size);;
    Record r_read = Record();

    fixed_len_write(&r, buffer);

    fixed_len_read(buffer, size, &r_read);
    cout << "\nRead In Serialized Record:\n" << endl;
    for (Record::iterator iter = (r_read).begin(); iter != (r_read).end(); iter++) {
        cout << *iter << " ";
    }
}
