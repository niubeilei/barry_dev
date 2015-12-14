#include "aosUtil/Types.h"
#include "Debug/Debug.h"
#include "Util/String.h"
#include "Util/IpAddr.h"
#include "aosApi.h"

bool api_torturer_special_case() 
{
    aos_iac_off();
		aos_iac_app_clear();
		aos_iac_timerule_clear();
		aos_iac_group_clear();
		aos_iac_base_clear();
		aos_iac_all_clear();

    // Command 1 Incorrect: name too long. Max: 64.     
    cout << aos_iac_remove_group("I(bYyw,*.IsjSia@=I=O}m83UOB9,l3|r>#86U@l}^VRGa<cSW3REs/DCx[w18t$U@c") << endl;


    // Command 2 Incorrect: name too long. Max: 64.     
    cout << aos_iac_gen_group("zb,^6c:2Q.iuybz<q4Y?3yKooB;.ud|po)wO53`fhr'-~JG;(#$9ENR5:lj1Pg!A:x:vTC(f7-r6!cO(d{@RJ<1c)z=#b>") << endl;


    // Command 3 Incorrect: name not in the table. .     
    cout << aos_iac_remove_app("HCD^x7=xD6~)2hZ&") << endl;


    // Command 4 Correct    
    cout << aos_iac_on() << endl;


    // Command 5 Correct    
    cout << aos_iac_gen_timerule("V<6k`?pF{") << endl;


    // Command 6 Incorrect: name too long. Max: 32.     
    cout << aos_iac_gen_app("Ddvu@`ym./I->w-XVvbu2*<r<[[p(,?J:6iX?cou<:#XZV") << endl;


    // Command 7 Incorrect: name too long. Max: 32.     
    cout << aos_iac_gen_app("z+qWDw-bCEBUG+:8~TyJVT!2ewv`!fLz:>{)7*4$vTWh*;IQ9C{nvF") << endl;


    // Command 8 Correct    
    {
        iac_group_conf_t arg0[100];
int arg1 = 100;

        cout << aos_iac_group_retrieve(arg0, &arg1) << endl;
    }

    // Command 9 Correct    
    cout << aos_iac_gen_app("y") << endl;


    // Command 10 Incorrect: name too long. Max: 64.     
    cout << aos_iac_remove_group("wZRu[;}AcPXOuqFedtMNZ^H_jOWs4YN,^J#;/IZ;C]i9NXH]Nt,R{|2gL4&_l{5KG_0TQ4oto$.h[TE+rx'ou7!kr&t`!*Tp2cobu*V0,/vf-<:~6?9T~bAqf6z1gO#w[;%QCZaM3#^>>vf{`}P*`;DoO@!`9KWte[o)`P}qR[0:|u6(>00HJRaC<,%TU&rd+") << endl;


    // Command 11 Incorrect: name not in the table. .     
    cout << aos_iac_remove_group("X:^<`|yfd*?U{<8iVBlTB;GOnSe#YC(3'/Mf,Gu9x") << endl;


    // Command 12 Correct    
    cout << aos_iac_gen_app("'_yptO3X@bG=pIu]xQb(HqZ93=3p,Q5") << endl;


    // Command 13 Correct    
    cout << aos_iac_add_timectrl("V<6k`?pF{", "14:5:16", "15:56:38", "Mon") << endl;


    // Command 14 Correct    
    cout << aos_iac_gen_group("bS;R*kf-n?!fEQak.$CYlF-xmAU5q7sS4WP<D`H[}qCDC") << endl;


    // Command 15 Incorrect: IacStatus == off does not hold. Value stored: on. Value expected: off    
    cout << aos_iac_on() << endl;


    // Command 16 Correct    
    {
        iac_time_conf_t arg0[100];
int arg1 = 100;

        cout << aos_iac_time_retrieve(arg0, &arg1) << endl;
    }

    // Command 17 Incorrect: Record does not exist in the table    
    cout << aos_iac_remove_flow("6^X!47qrSn'-?kO~],GNv5qDmBr3tWGS", "udp", 3261234220, 4244635647, 27931, 30096) << endl;


    // Command 18 Incorrect: 20:30:51 <= 15:56:38 does not hold. 1    
    cout << aos_iac_add_timectrl("V<6k`?pF{", "20:30:51", "15:56:38", "Mon") << endl;


    // Command 19 Correct    
    {
        iac_time_conf_t arg0[100];
int arg1 = 100;

        cout << aos_iac_time_retrieve(arg0, &arg1) << endl;
    }

    // Command 20 Incorrect: start_timeis syntactically incorrect.     
    cout << aos_iac_add_timectrl("V<6k`?pF{", "", "13:19:27", "Mon") << endl;


    // Command 21 Correct    
    {
        iac_time_conf_t arg0[100];
int arg1 = 100;

        cout << aos_iac_time_retrieve(arg0, &arg1) << endl;
    }

    // Command 22 Incorrect: name is in the table. .     
    cout << aos_iac_gen_group("bS;R*kf-n?!fEQak.$CYlF-xmAU5q7sS4WP<D`H[}qCDC") << endl;


    // Command 23 Correct    
    {
        iac_time_conf_t arg0[100];
int arg1 = 100;

        cout << aos_iac_time_retrieve(arg0, &arg1) << endl;
    }

    // Command 24 Correct    
    cout << aos_iac_remove_group("bS;R*kf-n?!fEQak.$CYlF-xmAU5q7sS4WP<D`H[}qCDC") << endl;


    // Command 25 Incorrect: StartPort < EndPort does not hold.     
    cout << aos_iac_add_flow("y", "TCP", 823173662, 255, 52540, 6824) << endl;


    // Command 26 Incorrect: ipis syntactically incorrect.     
    cout << aos_iac_add_flow("'_yptO3X@bG=pIu]xQb(HqZ93=3p,Q5", "TCP", 138257388, 4294967295, 64141, 23378) << endl;


    // Command 27 Correct    
    cout << aos_iac_gen_timerule("/aE[>99T;%o}7dIoXCeMwTMGiW,Dr,N") << endl;


    // Command 28 Incorrect: Command identifies an entry.     
    cout << aos_iac_add_timectrl("V<6k`?pF{", "14:5:16", "15:56:38", "Mon") << endl;


    // Command 29 Incorrect: IacStatus == off does not hold. Value stored: on. Value expected: off    
    cout << aos_iac_on() << endl;


    // Command 30 Incorrect: Record exist in the table    
    cout << aos_iac_add_timectrl("V<6k`?pF{", "14:5:16", "15:56:38", "Mon") << endl;


    // Command 31 Correct    
    cout << aos_iac_remove_app("'_yptO3X@bG=pIu]xQb(HqZ93=3p,Q5") << endl;


    // Command 32 Correct    
    cout << aos_iac_add_flow("y", "UDP", 2788094766, 4278190079, 25941, 52944) << endl;


    // Command 33 Correct    
    {
        iac_group_conf_t arg0[100];
int arg1 = 100;

        cout << aos_iac_group_retrieve(arg0, &arg1) << endl;
    }

    // Command 34 Correct    
    {
        iac_group_conf_t arg0[100];
int arg1 = 100;

        cout << aos_iac_group_retrieve(arg0, &arg1) << endl;
    }

    // Command 35 Incorrect: ParmInt.cpp:422 bad value. StartPort: bad value: -2147483648.     
    cout << aos_iac_remove_flow("y", "udp", 3107692567, 4278190079, -2147483648, 47031) << endl;


    // Command 36 Incorrect: IacStatus == off does not hold. Value stored: on. Value expected: off    
    cout << aos_iac_on() << endl;


    // Command 37 Incorrect: proto too long. Max: 32.     
    cout << aos_iac_add_flow("y", "#[b:I8xlLjit%[[@lLo7w*a(DjlPSYu~_X9)o[?fo)Zrca3y.K1'SpUA&Bq", 1959364119, 4278190079, 28157, 52944) << endl;


    // Command 38 Correct    
    cout << aos_iac_gen_app("N[L'(]T%v+E{t`};;Ih&->QK[;$#Q") << endl;


    // Command 39 Correct    
    cout << aos_iac_remove_app("y") << endl;


    // Command 40 Correct    
    cout << aos_iac_gen_app("M(GVtlK}5e:]v>46n^;R+<M") << endl;


    // Command 41 Incorrect: name too long. Max: 64.     
    cout << aos_iac_remove_group("=i%=_oh]NvuTn]gMKngGxLtO/6XpO<L6'ORdidj8Za6r>|@46PYWFN)T/+n]F;qKjDXS)C5.MI~koiH'c") << endl;


    // Command 42 Correct    
    {
        iac_group_conf_t arg0[100];
int arg1 = 100;

        cout << aos_iac_group_retrieve(arg0, &arg1) << endl;
    }

    // Command 43 Correct    
    cout << aos_iac_gen_group(")bu/)Y~{Dm}A?ATt9H?M8)tfjum>:Enj(dwW?BS-0QMv=K7}rTKT[@;o_QWxAFcG") << endl;


    // Command 44 Incorrect: StartPort < EndPort does not hold.     
    cout << aos_iac_add_flow("M(GVtlK}5e:]v>46n^;R+<M", "TCP", 2788094766, 16711679, 59952, 55642) << endl;


    // Command 45 Correct    
    {
        iac_group_conf_t arg0[100];
int arg1 = 100;

        cout << aos_iac_group_retrieve(arg0, &arg1) << endl;
    }

    // Command 46 Correct    
    cout << aos_iac_gen_app("{}Xy%#7Jp_I3Gh:2??n}n") << endl;


    // Command 47 Incorrect: 13:32:9 <= 12:16:33 does not hold. 1    
    cout << aos_iac_add_timectrl("V<6k`?pF{", "13:32:9", "12:16:33", "Sun") << endl;


    // Command 48 Correct    
    cout << aos_iac_add_timectrl("/aE[>99T;%o}7dIoXCeMwTMGiW,Dr,N", "11:53:48", "15:56:38", "Sun") << endl;


    // Command 49 Correct    
    cout << aos_iac_remove_app("M(GVtlK}5e:]v>46n^;R+<M") << endl;


    // Command 50 Incorrect: day too long. Max: 32.     
    cout << aos_iac_add_timectrl("/aE[>99T;%o}7dIoXCeMwTMGiW,Dr,N", "00:59:23", "14:59:4", "zZ8{!2nX?j~T1#daR{FpI>!`.wA-zBt@{-fGfT~&?HXxrf%n.I+~f+_r$IIGjgf1=LV$J!(iq*-e90SEX]Dh2L[4tNY*6i:Q`oS+:%=-UH") << endl;

	return true;
}

