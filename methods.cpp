#include <cstdlib>

// TODO - skoro svim dodjelama treba revidirati
//        da li je ona var kojoj dodjeljujem fakat var?
//        ako nije var onda je izraz, tj *a
//        onda treba upisati u [ebx]


bool Assignable(int position) {
    std::wstring name = GetNodeName(position);

    return name == std::wstring(L"VAR") ||
           name == std::wstring(L"INDEX") ||
           name == std::wstring(L"PTR");
}

bool IsDeclaredLocal(Name name1)
{  std::wstring name=std::wstring(name1);
    std::string::size_type found;
    std::wstring decls[4]={L"EXTRN",L"LARRDEF",L"LVARDEF",L"FPARAM"};
    for (int i=0;i<4;i++) {
        found = ParseList.find(L"("+decls[i]+L" "+name+L" ",scopepos);
        if (found!=std::string::npos)
            return true;
    }
    return false;
}

bool IsAuto(Name name1)
{  std::wstring name=std::wstring(name1);
    std::string::size_type found;
    std::wstring decls[3]={L"LARRDEF",L"LVARDEF",L"FPARAM"};
    for (int i=0;i<3;i++) {
        found = ParseList.find(L"("+decls[i]+L" "+name+L" ",scopepos);
        if (found!=std::string::npos)
            return true;
    }
    return false;
}

bool IsDeclaredGlobal(Name name1)
{
    std::wstring name=std::wstring(name1);
    std::string::size_type found;
    std::wstring decls[4]={L"GARRDEF",L"GVARDEF",L"FUNCDEF"};
    for (int i=0;i<4;i++) {
        found = ParseList.find(L"("+decls[i]+L" "+name+L" ",0);
        if (found!=std::string::npos)
            return true;
    }
   
    return false;
}

void UndecErr  (Name name)
{
    Name msg;
    wcscpy(msg,L"Undeclared ");
    wcscat(msg,name);
    SemErr (msg);
}

void AlreadyErr  (Name name)
{
    Name msg;
    wcscpy(msg,L"Already declared ");
    wcscat(msg,name);
    SemErr (msg);
}

std::wstring GetNodeName (int position)
{
    wstring NodeName;
    NodeName=L"";
    if (ParseList[position] != '(')
        return L"";
    position++;
    while (ParseList[position] >='A' && ParseList[position] <='Z' )
    {  NodeName+=ParseList[position];
        position++;
    }
    return NodeName;
}

std::wstring GetElemName (int position, int elemnum)
{
    wstring ElemName;
    position=ElemPos(position,elemnum);
    ElemName=L"";
    bool escaped = false;
    if (position>0)
      if ( ParseList[position] == '`' ) {
          escaped = !escaped;
      }

      while (escaped || (ParseList[position] !=' ' && ParseList[position] !=')') )
      { 
          ElemName+=ParseList[position];
          position++;
          if ( ParseList[position] == '`' ) {
              escaped = !escaped;
          }
      }
    return ElemName;
}

int ElemPos(int position, int elemnum)
{
    int currel,brlevel;
    currel=0;
    position++;
    while (ParseList[position] >='A' && ParseList[position] <='Z' )
        position++;
    for (;;)
    {
        while (ParseList[position] ==' ' )
            position++;
        currel++;
        if (currel==elemnum && ParseList[position] !=')')
            return position;
        switch (ParseList[position])
        {
            case '(':
                brlevel=1;
                position++;
                while (brlevel !=0)
                {
                    while (ParseList[position] !='(' && ParseList[position] !='`'  && ParseList[position] !=')')
                        position++;
                    switch(ParseList[position])
                    {
                        case '(':
                            brlevel++;position++;
                            break;
                        case ')':
                            brlevel--;position++;
                            break;
                        case '`':
                            do {position++;} while (ParseList[position]!='`');position++;
                            break;
                    }
                }
                break;
            case ')':
                return -1;
                break;
            case '`':
                do
                {
                    position++;
                   
                } while (ParseList[position]!='`');
                position++;
                break;
            default:
                while (ParseList[position] !='(' && ParseList[position] !=' '  && ParseList[position] !=')')
                  position++;
        }
    }
   
}


/**
 * Ispise asm kod tako da ako vrijedi proslijedjena uslovna jump instrukcija, u eax bude 0
 * u protivnom 1. Npr CmpToBool( L"jl", text ) ce upisati 1 u eax ako je A < B
 */
void CmpToBool(const std::wstring& jinstr, std::wstringstream& text) {
    auto zero = NextLabel();
    auto one = NextLabel();

    text << L" " << jinstr << L" " << zero << std::endl
         << L" mov eax, dword 0" << std::endl
         << L" jmp " << one << std::endl
         << L" " << zero << L":" << std::endl
         << L" mov eax, dword 1" << std::endl
         << L" " << one << L":" << std::endl;
}

/**
 * Za generisanje binarnih operacija +,-,*...
 */
void BinaryOp(int position, std::wstring opInst,
              std::wstringstream& data, std::wstringstream& bss, std::wstringstream& text) {
    auto z = ElemPos(position,1);
    auto q = ElemPos(position,2);

    if ( opInst == L"MUL" ) {
        opInst = L"imul";
    } else if ( opInst == L"DIV" ) {
        opInst = L"idiv";
    }

    Compile(z, data, bss, text);;
    if (GetNodeName(q)==L"INT")
    {
        if ( opInst == L"idiv" ) {
            text << L" cdq" << std::endl;
        }
        text << L" " << opInst << L" "<< GetElemName(q,1)<< endl ;
    } else if (GetNodeName(q)==L"VAR")
    {
        if ( opInst == L"idiv" ) {
            text << L" cdq" << std::endl;
        }
        text << L" " << opInst << L" dword [" << GetLocalVar( GetElemName( q , 1 ) ).GetName() << "]"<< endl;
    } else
    {
        text << L" PUSH eax" << std::endl;
        Compile(q, data, bss, text);;
        text << L" mov ebx, eax" << std::endl;
        text << L" POP eax" << std::endl;
        if ( opInst == L"idiv" ) {
            text << L" cdq" << std::endl;
        }
        text << L" " << opInst << L" EBX"<< endl;
    }
}


/**
 * Za generisanje addmov/divmov/submov/etc
 */
void OpMov(int position, const std::wstring& opInstr,
           std::wstringstream& data, std::wstringstream& bss, std::wstringstream& text) {
    // X op Y
    // z je pozicija od X
    // q je pozicija od Y
    auto z = ElemPos(position, 1);
    auto q = ElemPos(position, 2);

    text << "; opmov " << endl;
    if ( GetNodeName(z) == L"VAR" ) {
        text << L" lea ebx, [" << GetLocalVar( GetElemName( z , 1 ) ).GetName() << "]" << std::endl;
        text << L" mov eax, [ebx]" << std::endl;
        text << L" push ebx" << std::endl;

        if (GetNodeName(q) == L"INT") {
            // X op const
            if ( opInstr == L"idiv" ) {
                text << L" cdq" << std::endl;
                text << " mov eax, dword " << GetElemName(q, 1) << endl;
                text << L" " << opInstr << L" eax" << endl;
            } else {
                text << L" " << opInstr << L" eax, dword " << GetElemName(q, 1) << endl;
            }
        } else if (GetNodeName(q) == L"VAR") {
            // X op var
            if ( opInstr == L"idiv" ) {
                text << L" cdq" << std::endl;
                text << L" " << opInstr << L" [" << GetLocalVar( GetElemName( q , 1 ) ).GetName() << "]" << endl;
            } else {
                text << L" " << opInstr << L" eax, [" << GetLocalVar(GetElemName(q, 1)).GetName() << "]" << endl;
            }
        } else {
            // X op expr
            text << L" push eax" << std::endl;
            Compile(q, data, bss, text);
            text << L" pop ebx" << std::endl;
            if ( opInstr == L"idiv" ) {
                text << L" cdq" << std::endl;
                text << " mov ecx, ebx" << std::endl;
                text << " mov ebx, eax" << std::endl;
                text << " mov eax, ecx" << std::endl;

                text << L" " << opInstr << L" eax" << endl;
            } else {
                text << L" " << opInstr << L" ebx, eax" << endl
                     << L" mov eax, ebx" << std::endl;
            }
        }

        // rezultat izraza treba da bude vrijednost varijable
        text << L" pop ebx" << std::endl;
        text << L" mov [ebx], eax" << std::endl << "; end of opmov" << std::endl;
    } else {
        text << L" todo op mov " << std::endl;
    }
}


struct LocalVariable {
    std::wstring name;
    std::wstring function;

    bool isArray;
    int length;

    LocalVariable(const std::wstring& function, const std::wstring& name) {
        this->name = name;
        this->function = function;
        this->isArray = false;
        this->length = 1;
    }


    LocalVariable(const std::wstring& function, const std::wstring& name, int len) {
        this->name = name;
        this->function = function;
        this->isArray = true;
        this->length = len;
    }

    int GetSize() const {
        return isArray ? 4 * length : 4;
    }

    std::wstring GetName() const {
        std::wstringstream ss;
        if ( function != L"" ) {
            ss << function << "_";
        }
        ss << name;

        return ss.str();
    }
};

int CalculateVarsSize(const std::vector<LocalVariable>& vars) {
    int total = 0;
    for ( const auto& n : vars ) {
        total += n.GetSize();
    }
    return total;
}

std::map< std::wstring, std::vector< std::wstring > > FunctionExterns;
std::map< std::wstring, std::vector< LocalVariable > > FunctionParameters;
std::map< std::wstring, std::vector< LocalVariable > > FunctionLocals;

std::vector<std::wstring> SwitchLabels;

std::wstring CurrentFunction;
unsigned int CurrentLoop = 0;
unsigned int CurrentSwitch = 0;
int isAssignableExpression = 0;

int CurrentArgCounter = 0;

std::wstring CurrentLoopStartLabel[256];
std::wstring CurrentLoopEndLabel[256];

std::wstring CurrentSwitchEndLabel[256];
std::wstring CurrentSwitchDefaultLabel[256];
std::wstring CurrentSwitchCaseLabel[256];
std::wstring PendingSwitchCaseLabel[256];


LocalVariable GetLocalVar(const std::wstring& elemname, bool isArray = false, int arrLen = 1) {
    bool isLocal = false;
    for ( const auto& n : FunctionLocals[CurrentFunction] ) {
        if ( n.name == elemname ) {
            isLocal = true;
            break;
        }
    }

    for ( const auto& n : FunctionParameters[CurrentFunction] ) {
        if ( n.name == elemname ) {
            isLocal = true;
            break;
        }
    }

    const auto& vec = FunctionExterns[CurrentFunction];
    bool found = false;
    for ( const auto& str : vec ) {
        if ( str != elemname ) {
            continue;
        }

        found = true;
        break;
    }

    if ( !isLocal && !found ) {
        SemErr(L"Global var not declared extern");
    }

    if ( isArray ) {
        return LocalVariable(isLocal ? CurrentFunction : L"", elemname, arrLen);
    }
    return LocalVariable(isLocal ? CurrentFunction : L"", elemname);
}


void PrintShift(int z, int q, const std::wstring& setInst, std::wstringstream& data,
        std::wstringstream& bss, std::wstringstream& text) {
    Compile(z, data, bss, text);
    text << " push eax" << std::endl;

    if ( GetNodeName(q) == L"INT" ) {
        text << " " << setInst << " eax, byte " << GetElemName(q, 1) << std::endl;
    } else {
        SemErr(L"Shifting requires a constant shift count");
    }
    Compile(q, data, bss, text);
    text << " pop ebx" << std::endl;
}


/**
 * Pomocna za CompMov instrukcije (IsGreaterEquMov, IsEquMov, ... )
 */
void PrintCompMov(int z, int q, const std::wstring& setInst, std::wstringstream& data,
                  std::wstringstream& bss, std::wstringstream& text) {
    PrintCompare(z, q, data, bss, text);

    if ( GetNodeName(z) == L"VAR" ) {
        text << " lea ebx, [" << GetLocalVar(GetElemName(z, 1)).GetName() << "]" << std::endl;
        text << " " << setInst << " [ebx]" << std::endl;
    } else if ( isAssignableExpression ) {
        text << " " << setInst << " [eax]" << std::endl;
    } else {
        text << " todo CompMov ";
    }
}

void PrintCompare(int z, int q, std::wstringstream& data, std::wstringstream& bss, std::wstringstream& text) {
    auto zType = GetNodeName(z);
    auto qType = GetNodeName(q);

    if ( zType == L"VAR" ) {
        text << L" mov eax, [" << GetLocalVar( GetElemName(z, 1) ).GetName() << L"]" << std::endl;
    } else if ( zType == L"INT" ) {
        text << L" mov eax, dword " << GetElemName(z, 1) << std::endl;
    } else {
        Compile(z, data, bss, text);
    }

    text << L" cmp eax, ";

    if ( qType == L"VAR" ) {
        text << L"[" <<GetLocalVar( GetElemName(q, 1) ).GetName() << L"]" << std::endl;
    } else if ( qType == L"INT" || qType == L"ONUMBER" ) {
        text << L"dword " << GetElemName(q, 1) << std::endl;
    } else {
        text << L"TODO PrintCompare - qType >" << qType << "<" << std::endl;
    }

}

/**
 * Generator pomocnih labela za "male skokove" (petlje, uvjetni skokovi i sl)
 */
std::wstring NextLabel() {
    static int cnt = 0;
    ++cnt;

    std::wstringstream ss;
    ss << L"_tmp_label_" << cnt;

    return ss.str();
}

void Compile(int position, std::wstringstream& data, std::wstringstream& bss, std::wstringstream& text)
{
    wstring nodename,elemname;
    int par,z,q;
    nodename=GetNodeName(position);
    par=1;

    if (nodename== L"ADD") {
        z=ElemPos(position,1);
        q=ElemPos(position,2);
        Compile(z, data, bss, text);;
        if (GetNodeName(q)==L"INT")
        {
              text << L" ADD EAX, dword "<< GetElemName(q,1)<< endl ;
        }
        else
        if (GetNodeName(q)==L"VAR")
        {
              text << L" ADD EAX,[" << GetLocalVar( GetElemName( q , 1 ) ).GetName() << "]"<< endl;
        }
        else
        {
            text << L" PUSH EAX" << std::endl;
            Compile(q, data, bss, text);;
            text << L" POP EBX" << std::endl;
            text << L" ADD EAX,EBX"<< endl;
        }
    }
    else if (nodename== L"ADDMOV") {
        OpMov(position, L"add", data, bss, text);
    }
    else if (nodename== L"ADDROF") {
        z = ElemPos(position, 1);

        if ( GetNodeName(z) == L"VAR" ) {
            text << " lea eax, [" << GetLocalVar( GetElemName( z , 1 ) ).GetName() << "]" << std::endl;
        } else {
            text << "TODO - ADDROF" << endl;
        }
    }
    else if (nodename== L"ANDMOV") {
        OpMov(position, L"and", data, bss, text);
    }
    else if (nodename== L"ARG") {
        z = ElemPos(position, 1);

        auto zType = GetNodeName(z);

        Compile(z, data, bss, text);
        text << " mov [esp + " << CurrentArgCounter << "], eax " << std::endl;
        CurrentArgCounter += 4;

    }
    else if (nodename== L"ASIZE") {
        // ASIZE should be handled in GARRDEF and ARRDEF
        text<<"TODO - ASIZE" << endl;
    }
    else if (nodename== L"BLOCK") {
        while ((z=ElemPos(position,par)) >0)
        {
          Compile(z, data, bss, text);
          par++;
        }
    }
    else if (nodename== L"BREAK") {
        text << " jmp "
             << (CurrentLoop ? CurrentLoopEndLabel[CurrentLoop]  : CurrentSwitchEndLabel[CurrentSwitch] )
             << std::endl;
    }
    else if (nodename== L"B" ) {
        while ((z=ElemPos(position,par)) >0)
        {
          Compile(z, data, bss, text);;
          par++;
        }
    }
    else if (nodename== L"CHAR") {
        auto val = GetElemName(position, 1);

        // val je karakter unutar `''` npr `'a'`
        std::wstringstream ss;
        ss << val[2];
        wchar_t sc;
        ss >> sc;

        char ascii = static_cast<char>(sc);
        text << " mov eax, dword " << static_cast<int>(ascii) << std::endl;
    }
    else if (nodename== L"CONTINUE") {
        text << " jmp " << CurrentLoopStartLabel[CurrentLoop]  << std::endl;
    }
    else if (nodename== L"DECLSTAT") {
        while ((z=ElemPos(position,par)) >0)
        {
          Compile(z, data, bss, text);;
          par++;
        }
    }
    else if (nodename== L"DIVMOV") {
        OpMov(position, L"idiv", data, bss, text);
    }
    else if (nodename== L"EQU" || nodename == L"NEQU") {
        z = ElemPos(position, 1);
        q = ElemPos(position, 2);

        PrintCompare(z, q, data, bss, text);

        CmpToBool(nodename == L"EQU" ? L"je" : L"jne", text);
    }
    else if (nodename== L"EXTRN") {
        FunctionExterns[CurrentFunction].push_back( GetElemName(position, 1) );
    }
    else if (nodename== L"FHEADER") {
        elemname = GetElemName(position, 1);

        CurrentArgCounter = 0;
        while ((z=ElemPos(position,par)) >0)
        {
            ++CurrentArgCounter;

            Compile(z, data, bss, text);;
            par++;
        }
    }
    else if (nodename== L"FPARAM") {
        elemname = GetElemName(position, 1);

        FunctionParameters[CurrentFunction].push_back(LocalVariable(CurrentFunction, elemname));

        text << " %define " << GetLocalVar(elemname).GetName() << " ebp+"
             << (4 + (FunctionParameters[CurrentFunction].size() * 4)) << endl;

    }
    else if (nodename== L"FUNCCALL") {
        int n = 2;
        z = ElemPos(position, n++);

        auto tmp = NextLabel();
        text << " sub esp, dword " << tmp << std::endl;
        CurrentArgCounter = 0;
        while ( GetNodeName(z) == L"ARG" ) {
            Compile(z, data, bss, text);
            z = ElemPos(position, n++);
        }
        text << " " << tmp << " equ " << CurrentArgCounter << std::endl;

        z = ElemPos(position, 1);
        if ( GetNodeName(z) == L"VAR" ) {
            text << " call _" << GetElemName(z, 1) << std::endl;
        } else {
            text << " calling something else than VAR? " << GetNodeName(z) << std::endl;
        }
    }
    else if (nodename== L"FUNCDEF") {
        elemname = L"_" + GetElemName(position,1);
        CurrentFunction = elemname;

        text << L" GLOBAL "<< elemname<< endl;
        text << endl << elemname<<":" << endl;
        text << L" PUSH EBP"<<endl<< L" MOV EBP,ESP"<<endl << L" SUB ESP,"<<elemname<<L"_len"<<endl;

        // funcdef must be followed by a fheader
        Compile (ElemPos(position,2), data, bss, text);

        // and then by a block
        Compile (ElemPos(position,3), data, bss, text);

        text << CurrentFunction << L"_end:" << endl;

        text << L" MOV ESP,EBP" << endl
             << L" POP EBP" << endl
             << L" RET " << (FunctionParameters[CurrentFunction].size() * 4) << endl;

        // add param length data entry
        data << elemname << "_len EQU " << (CalculateVarsSize(FunctionLocals[CurrentFunction])) << endl;
    }
    else if (nodename== L"GARRDEF") {
        elemname = GetElemName(position, 1);

        auto arrSize = ElemPos(position, 2);
        if ( GetNodeName(arrSize) != L"ASIZE" ) {
            text << " todo - asize isnt here?" << GetNodeName(arrSize) << std::endl;
        }

        int arrLen = 0;
        std::wstringstream ss;
        ss << GetElemName(arrSize, 1);
        ss >> arrLen;

        bss << elemname << " resd " << (arrLen * 4) << std::endl;
    }
    else if (nodename== L"GOTO") {
        elemname = GetElemName(position, 1);

        text << L" jmp label_" << elemname << endl;
    }
    else if (nodename== L"GREATEREQUTHAN") {
        z = ElemPos(position, 1);
        q = ElemPos(position, 2);

        PrintCompare(z, q, data, bss, text);

        CmpToBool(L"jge", text);
    }
    else if (nodename== L"GREATERTHAN") {
        z = ElemPos(position, 1);
        q = ElemPos(position, 2);

        PrintCompare(z, q, data, bss, text);

        CmpToBool(L"jg", text);
    }
    else if (nodename== L"GVARDEF") {
        elemname = GetElemName(position, 1);

        bss << elemname << " resd 4" << endl;
    }
    else if (nodename== L"WHILE") {
        CurrentLoop ++;

        int condblock = ElemPos(position, 1);
        int bodyBlock = ElemPos(position, 2);

        CurrentLoopStartLabel[CurrentLoop]  = NextLabel();
        CurrentLoopEndLabel[CurrentLoop]  = NextLabel();

        text << CurrentLoopStartLabel[CurrentLoop]  << L":" << std::endl;
        Compile(condblock, data, bss, text);

        text << " jz " << CurrentLoopEndLabel[CurrentLoop]  << std::endl;

        Compile(bodyBlock, data, bss, text);

        text << L" jmp " << CurrentLoopStartLabel[CurrentLoop]  << std::endl;

        text << CurrentLoopEndLabel[CurrentLoop]  << ":" << std::endl;

        CurrentLoop --;
    }
    else if ( nodename == L"IFELSE" || nodename == L"IF" || nodename == L"CONDEXPR" ) {
        int condblock = ElemPos(position, 1);
        int ifblock = ElemPos(position, 2);
        int elseblock = ElemPos(position, 3);
        bool hasElse = nodename == L"IFELSE" || nodename == L"CONDEXPR";

        auto endiflabel = NextLabel();
        auto elselabel = hasElse ? NextLabel() : endiflabel;

        Compile(condblock, data, bss, text);

        text << " jz " << elselabel << std::endl;
        Compile(ifblock, data, bss, text);

        if ( hasElse ) {
            text << " jmp " << endiflabel << std::endl;

            text << elselabel << L":" << std::endl;

            Compile(elseblock, data, bss, text);
        }

        text << endiflabel << ":" << std::endl;
    }
    else if (nodename== L"INDEX") {
        z = ElemPos(position, 1);
        q = ElemPos(position, 2);

        auto zType = GetNodeName(z);
        auto qType = GetNodeName(q);

        if ( zType == L"VAR" ) {
            text << " lea eax, [" << GetLocalVar( GetElemName(z, 1) ).GetName() << "]" << std::endl;
        } else {
            Compile(z, data, bss, text);
        }

        if ( qType == L"INT" || qType == L"ONUMBER" ) {
            text << " push eax" << std::endl;
            text << " mov eax, dword " << GetElemName(q, 1) << std::endl;
            text << " sal eax, 2" << std::endl;
            text << " pop ebx" << std::endl;
            text << " add eax, ebx" << std::endl;
        } else {
            text << " push eax" << std::endl;

            Compile(q, data, bss, text);

            text << " sal eax, 2" << std::endl;

            text << " mov ebx, eax" << std::endl;
            text << " pop eax" << std::endl;

            text << " add eax, ebx" <<std::endl;
        }

        text << " mov ebx, eax" << std::endl;
        text << " mov eax, [eax]" << std::endl;
    }
    else if (nodename== L"INIT") {
        text << " todo - init " << std::endl;
        text << " mov eax, dword " << GetElemName(position, 1) << std::endl;
    }
    else if (nodename== L"INT") {
        text << L" MOV EAX, dword "<< GetElemName(position,1) << std::endl;
    }
    else if (nodename== L"ISEQUMOV") {
        z = ElemPos(position, 1);
        q = ElemPos(position, 2);

        z = ElemPos(position, 1);
        q = ElemPos(position, 2);

        PrintCompMov(z, q, L"sete", data, bss, text);
    }
    else if (nodename== L"ISGREATEREQUMOV") {
        z = ElemPos(position, 1);
        q = ElemPos(position, 2);

        PrintCompMov(z, q, L"setge", data, bss, text);
    }
    else if (nodename== L"ISGREATERMOV") {
        z = ElemPos(position, 1);
        q = ElemPos(position, 2);

        PrintCompMov(z, q, L"setg", data, bss, text);
    }
    else if (nodename== L"ISLESSEQUMOV") {
        z = ElemPos(position, 1);
        q = ElemPos(position, 2);

        PrintCompMov(z, q, L"setle", data, bss, text);
    }
    else if (nodename== L"ISLESSMOV") {
        z = ElemPos(position, 1);
        q = ElemPos(position, 2);

        PrintCompMov(z, q, L"setl", data, bss, text);
    }
    else if (nodename== L"ISNEQUMOV") {
        z = ElemPos(position, 1);
        q = ElemPos(position, 2);

        PrintCompMov(z, q, L"setne", data, bss, text);
    }
    else if (nodename== L"LABEL") {
        elemname = GetElemName(position, 1);

        text << L"label_" << elemname << L":" << endl;
    }
    else if (nodename== L"LESSEQUTHAN") {
        z = ElemPos(position, 1);
        q = ElemPos(position, 2);

        PrintCompare(z, q, data, bss, text);

        CmpToBool(L"jle", text);
    }
    else if (nodename== L"LESSTHAN") {
        z = ElemPos(position, 1);
        q = ElemPos(position, 2);

        PrintCompare(z, q, data, bss, text);

        CmpToBool(L"jl", text);
    }
    else if (nodename== L"LSHIFTMOV") {
        OpMov(position, L"shl", data, bss, text);
    }
    else if (nodename== L"LARRDEF") {
        elemname = GetElemName(position, 1);
        z = ElemPos(position, 2);
        auto arrSize = 0;

        if ( GetNodeName(z) != L"ASIZE" ) {
            text << " todo - something else given in larrdef?" << GetNodeName(z) << std::endl;
        }

        std::wstringstream ss;
        ss << GetElemName(z, 1);
        ss >> arrSize;

        auto var = LocalVariable(CurrentFunction, elemname, arrSize);

        FunctionLocals[CurrentFunction].push_back( var );

        text << " %define " << var.GetName() << ""
             << " ebp-" << CalculateVarsSize(FunctionLocals[CurrentFunction]) << endl;



        z = ElemPos(position, 2);

        if ( GetNodeName(z) == L"INIT" ) {
            text << " mov [ebp - " << CalculateVarsSize(FunctionLocals[CurrentFunction]) << "], dword "
                 << GetElemName(z, 1) << endl;
        }
    }
    else if (nodename== L"LVARDEF") {
        elemname = GetElemName(position, 1);
        auto var = LocalVariable(CurrentFunction, elemname);

        FunctionLocals[CurrentFunction].push_back( var );
        text << " %define " << var.GetName() << ""
             << " ebp-" << CalculateVarsSize(FunctionLocals[CurrentFunction]) << endl;


        z = ElemPos(position, 2);

        if ( GetNodeName(z) == L"INIT" ) {
            text << " mov [ebp - " << CalculateVarsSize(FunctionLocals[CurrentFunction]) << "], dword "
                 << GetElemName(z, 1) << endl;
        }
    }
    else if (nodename== L"MOD") {
        z = ElemPos(position, 1);
        q = ElemPos(position, 1);

        auto zType = GetNodeName(z);
        auto qType = GetNodeName(q);


        if ( zType == L"VAR" ) {
            auto zName = GetElemName(z, 1);
            text << " mov eax, [" << GetLocalVar(zName).GetName() << "]" << std::endl;
        } else {
            Compile(z, data, bss, text);
        }

        text << " push eax " << std::endl;

        if ( qType == L"VAR" ) {
            auto qName = GetElemName(q, 1);
            text << " mov eax, [" << GetLocalVar(qName).GetName() << "]" << std::endl;
        } else {
            Compile(q, data, bss, text);
        }

        text << " pop ebx" << std::endl;
        text << " cdq" << std::endl;
        text << " idiv eax, ebx" << std::endl;
        text << " mov eax, edx" << std::endl;
    }
    else if (nodename== L"MODMOV") {
        z = ElemPos(position, 1);
        q = ElemPos(position, 1);

        auto zType = GetNodeName(z);
        auto qType = GetNodeName(q);

        if ( zType == L"VAR" ) {
            auto zName = GetElemName(z, 1);
            text << " mov eax, [" << GetLocalVar(zName).GetName() << "]" << std::endl;
        } else {
            Compile(z, data, bss, text);
        }

        text << " push eax " << std::endl;

        if ( qType == L"VAR" ) {
            auto qName = GetElemName(q, 1);
            text << " mov eax, [" << GetLocalVar(qName).GetName() << "]" << std::endl;
        } else {
            Compile(q, data, bss, text);
        }

        text << " pop ebx" << std::endl;
        if ( isAssignableExpression ) {
            text << " push ebx" << std::endl;
        }

        text << " cdq" << std::endl;
        text << " idiv eax, ebx" << std::endl;
        text << " mov eax, edx" << std::endl;

        text << " pop ebx" << std::endl;
        if ( zType == L"VAR" ) {
            text << " mov [" << GetLocalVar(GetElemName(z, 1)).GetName() << "], eax" << std::endl;
        } else if ( isAssignableExpression ) {
            text << " mov [ebx], eax" << std::endl;
        } else {
            text << " todo - divmov " << std::endl;
        }
    }
    else if (nodename== L"MOV") {
        z=ElemPos(position, 1);
        q=ElemPos(position, 2);

        if (GetNodeName(z)==L"VAR")
        {
            if (GetNodeName(q)==L"INT")
            {
              text << L" MOV DWORD [" << GetLocalVar( GetElemName( z, 1) ).GetName() <<"],"
                   << GetElemName(q,1 ) << endl ;
            } else if (GetNodeName(q)==L"VAR")
            {
              text << L" MOV EAX, [" << GetLocalVar( GetElemName( q , 1 ) ).GetName() << "]"<< endl;
              text << L" MOV [" << GetLocalVar( GetElemName( z, 1 ) ).GetName() <<"],EAX"<< endl;
            } else if (GetNodeName(q) == L"ONUMBER") {
                text << L" xor eax, eax" << std::endl;
                text << L" mov [" << GetLocalVar( GetElemName( z, 1 ) ).GetName() <<"], EAX"<< endl;
            } else
            {
              Compile(q, data, bss, text);;
              text << L" MOV [" << GetLocalVar( GetElemName( z, 1 ) ).GetName() <<"],EAX"<< endl;
            }
        }
        else
        {
            if (GetNodeName(q)==L"INT")
            {
              Compile(z, data, bss, text);
              text << L" MOV DWORD [EBX],"<< GetElemName(q,1) << std::endl;
            }
            else
            if (GetNodeName(q)==L"VAR")
            {
              Compile(z, data, bss, text);
              text << L" MOV EAX, [" << GetLocalVar( GetElemName( q , 1 ) ).GetName() << "]"<< endl;
              text << L" MOV [EBX],EAX"<< endl;
            }
            else
            {
              Compile(q, data, bss, text);
              text << L" PUSH EAX       ; push expr result"<< endl;
              Compile(z, data, bss, text);
              text << L" POP EAX        ; pop expr result "<< endl;
              text << L" MOV [EBX], EAX"<< endl;
            }


        }
    }
    else if (nodename== L"MUL" || nodename == L"SUB" || nodename == L"DIV"
            || nodename == L"ADD" || nodename == L"OR" || nodename == L"XOR"
            || nodename == L"AND") {
        BinaryOp(position, nodename, data, bss, text);
    }
    else if (nodename== L"MULTMOV") {
        OpMov(position, L"imul", data, bss, text);
    }
    else if (nodename== L"ONUMBER") {
        text << " mov eax, dword 0; maybe this could've been optimized? onumber" << std::endl;
    }
    else if (nodename== L"ORMOV") {
        OpMov(position, L"or", data, bss, text);
    }
    else if (nodename == L"POSTDEC" || nodename== L"POSTINC") {
        z = ElemPos(position, 1);

        auto zType = GetNodeName(z);

        if ( zType == L"VAR" ) {
            text << " lea ebx, [" << GetLocalVar( GetElemName( z , 1 ) ).GetName() << "]" << std::endl;
            text << " mov eax, [ebx]" << std::endl;
            text << " mov edx, eax" << std::endl;

            if ( nodename == L"POSTDEC" ) {
                text << " dec eax" << std::endl;
            } else {
                text << " inc eax" << std::endl;
            }

            text << " mov [ebx], eax" << std::endl;
            text << " mov eax, edx" << std::endl;
        } else {
            // izraz
            text << "todo postdec/postinc" << zType << std::endl;
        }
    }
    else if (nodename== L"PREDEC" || nodename== L"PREINC") {
        z = ElemPos(position, 1);

        auto zType = GetNodeName(z);

        if ( zType == L"VAR" ) {
            auto zVar = GetLocalVar(GetElemName(z, 1));

            text << " lea ebx, [" << zVar.GetName() << "]" << std::endl;
            text << " mov eax, [ebx]" << std::endl;

            if ( nodename == L"PREDEC" ) {
                text << " dec eax" << std::endl;
            } else {
                text << " inc eax" << std::endl;
            }

            text << " mov [ebx], eax" << std::endl;
        } else {
            // izraz
            text << "todo postdec/postinc" << zType << std::endl;
        }
    }
    else if (nodename== L"PTR") {
        z = ElemPos(position, 1);

        auto zType = GetNodeName(position);

        Compile(z, data, bss, text);

        text << " mov eax, [eax]" << std::endl;
    }
    else if (nodename== L"RETURN") {
        text << " jmp " << CurrentFunction << "_end" << endl;
    }
    else if (nodename== L"RETURNPARAM") {
        z = ElemPos(position, 1);

        if ( GetNodeName(z) == L"INT" ) {
            text << L" mov eax, dword " << GetElemName(z, 1) << endl;
        } else if ( GetNodeName(z) == L"VAR" ) {
            text << L" mov eax, dword [" << GetLocalVar( GetElemName( z, 1 ) ).GetName() << L"]" << endl;
        } else {
            Compile(z, data, bss, text);
        }

        text << " jmp " << CurrentFunction << "_end" << endl;
    }
    else if (nodename== L"LSHIFT") {
        z = ElemPos(position, 1);
        q = ElemPos(position, 2);
        PrintShift(z, q, L"shl", data, bss, text);
    }
    else if (nodename== L"RSHIFT") {
        z = ElemPos(position, 1);
        q = ElemPos(position, 2);
        PrintShift(z, q, L"shr", data, bss, text);
    }
    else if (nodename== L"RSHIFTMOV") {
        OpMov(position, L"shr", data, bss, text);
    }
    else if (nodename== L"SAMEAS") {
        // todo
        text<<"TODO - SAMEAS" << endl;
    }
    else if (nodename== L"STRING") {
        auto name = NextLabel();

        auto literal = GetElemName(position, 1);

        data << "; " << literal << std::endl;
        data << name;

        for ( int i = 2; i < literal.size() - 2; ++ i ) {
            data << "   db " <<  static_cast<int>(literal[i]) << std::endl;
        }

        data << "   db 0 " << std::endl;
        data << std::endl;

        text << " lea eax, [" << name << "]" << std::endl;
    }else if (nodename== L"SUBMOV") {
        OpMov(position, L"sub", data, bss, text);
    } else if (nodename== L"DEFAULT") {
        text << CurrentSwitchDefaultLabel[CurrentSwitch]  << ":" << std::endl;
        CurrentSwitchDefaultLabel[CurrentSwitch] = L"";
    } else if ( nodename == L"CASE" ) {
        z = ElemPos(position, 1);

        auto zType = GetNodeName(z);

        if ( PendingSwitchCaseLabel[CurrentSwitch].size() != 0 ) {
            CurrentSwitchCaseLabel[CurrentSwitch] = PendingSwitchCaseLabel[CurrentSwitch];
        } else {
            CurrentSwitchCaseLabel[CurrentSwitch] = NextLabel();
        }

        text << CurrentSwitchCaseLabel[CurrentSwitch] << ": ; case " << GetElemName(z, 1) << std::endl;

        if ( zType == L"INT" || zType == L"ONUMBER" ) {
            auto val = GetElemName(z, 1);
            text << " pop eax" << std::endl;
            text << " push eax" << std::endl;
            text << " cmp eax, dword " << val << std::endl;
            PendingSwitchCaseLabel[CurrentSwitch] = NextLabel();
            text << " jnz " << PendingSwitchCaseLabel[CurrentSwitch] << std::endl;
        } else {
            SemErr(L"Only integral literals supported for switch cases. ");
            text << zType << " unsupported for case " << std::endl;
        }
    }
    else if (nodename== L"SWITCH") {
        CurrentSwitch ++;


        auto ref = ElemPos(position, 1);
        auto block = ElemPos(position, 2);

        auto evalBlock = NextLabel();
        CurrentSwitchEndLabel[CurrentSwitch]  = NextLabel();
        CurrentSwitchDefaultLabel[CurrentSwitch] = NextLabel();
        auto defaultLabel = CurrentSwitchDefaultLabel[CurrentSwitch];


        Compile(ref, data, bss, text);

        text << " push eax" << std::endl;

        Compile(block, data, bss, text);

        if ( PendingSwitchCaseLabel[CurrentSwitch].size() != 0 ) {
            text << PendingSwitchCaseLabel[CurrentSwitch] << ":" << std::endl;
            text << " jmp " << defaultLabel << std::endl;
            PendingSwitchCaseLabel[CurrentSwitch] = L"";
        }
        if ( CurrentSwitchDefaultLabel[CurrentSwitch].size() != 0 ) {
            text << defaultLabel << ":" << std::endl;
        }

        text << CurrentSwitchEndLabel[CurrentSwitch]  << ":" << std::endl;

        text << " pop eax" << std::endl;

        CurrentSwitch --;
    }
    else if (nodename== L"UNEG") {
        z = ElemPos(position, 1);


        if ( GetNodeName(z) == L"INT" ) {
            text << " mov eax, dword " << GetElemName(z, 1) << endl;
        } else if ( GetNodeName(z) == L"VAR" ) {
            text << " mov eax, [" << GetLocalVar( GetElemName( z , 1 ) ).GetName() << "]" << endl;
        } else {
            Compile(z, data, bss, text);
        }

        text << " neg eax" << std::endl;
    }
    else if (nodename== L"UNOT") {
        z = ElemPos(position, 1);


        if ( GetNodeName(z) == L"INT" ) {
            text << " mov eax, dword " << GetElemName(z, 1) << endl;
        } else if ( GetNodeName(z) == L"VAR" ) {
            text << " mov eax, [" << GetLocalVar( GetElemName( z , 1 ) ).GetName() << "]" << endl;
        } else {
            Compile(z, data, bss, text);
        }

        text << " not eax" << std::endl;
    }
    else if (nodename== L"UMINUS") {
        z = ElemPos(position, 1);

        auto zType = GetNodeName(z);

        Compile(z, data, bss, text);

        text << L" neg eax" << std::endl
             << L" mov [ebx], eax" << std::endl;
    }
    else if (nodename== L"UPLUS") {
        // Podrzan radi simetrije sa unarnim minusom, ali nema prave funkcije.
    }
    else if (nodename== L"VAR") {
        text << L" LEA EBX, [" << GetLocalVar(GetElemName(position,1) ).GetName() <<"]"<< endl;
        text << L" MOV EAX, [EBX]"<< endl;
    }
    else if (nodename== L"XORMOV") {
        OpMov(position, L"xor", data, bss, text);
    }
}