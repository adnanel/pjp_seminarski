

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
    if (position>0)
      while (ParseList[position] !=' ' && ParseList[position] !=')' )
      { 
          ElemName+=ParseList[position];
          position++;
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

void PrintCompare(int z, int q, std::wstringstream& text) {
    auto zType = GetNodeName(z);
    auto qType = GetNodeName(q);

    text << L" mov eax, ";
    if ( zType == L"VAR" ) {
        text << L"[" << GetElemName(z, 1) << L"]" << std::endl;
    } else if ( zType == L"INT" ) {
        text << L"dword " << GetElemName(z, 1) << std::endl;
    } else {
        text << L"TODO PrintCompare zType - " << zType << std::endl;
    }

    text << L" cmp eax, ";

    if ( qType == L"VAR" ) {
        text << L"[" << GetElemName(q, 1) << L"]" << std::endl;
    } else if ( qType == L"INT" || qType == L"ONUMBER" ) {
        text << L"dword " << GetElemName(q, 1) << std::endl;
    } else {
        text << L"TODO PrintCompare - qType >" << qType << "<" << std::endl;
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
        text << L" lea ebx, [" << GetElemName(z, 1) << "]" << std::endl;
        text << L" mov eax, [ebx]" << std::endl;
        text << L" push ebx" << std::endl;

        if (GetNodeName(q) == L"INT") {
            // X op const
            text << L" " << opInstr << L" eax, dword " << GetElemName(q, 1) << endl;;
        } else if (GetNodeName(q) == L"VAR") {
            // X op var
            text << L" " << opInstr << L" eax, [" << GetElemName(q, 1) << "]" << endl;
        } else {
            // X op expr
            text << L" push eax" << std::endl;
            Compile(q, data, bss, text);
            text << L" pop ebx" << std::endl;
            text << L" " << opInstr << L" ebx, eax" << endl
                 << L" mov eax, ebx" << std::endl;
        }

        // rezultat izraza treba da bude vrijednost varijable
        text << L" pop ebx" << std::endl;
        text << L" mov [ebx], eax" << std::endl;
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
        ss << function << "_" << name;

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

std::map< std::wstring, std::vector< std::wstring > > FunctionParameters;
std::map< std::wstring, std::vector< LocalVariable > > FunctionLocals;

std::wstring CurrentFunction;
bool isInsideLoop = false;
bool isInsideSwitch = false;

std::wstring CurrentLoopStartLabel;
std::wstring CurrentLoopEndLabel;

std::wstring CurrentSwitchStartLabel;
std::wstring CurrentSwitchEndLabel;

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
              text << L" ADD EAX,"<< GetElemName(q,1)<< endl ;
        }
        else
        if (GetNodeName(q)==L"VAR")
        {
              text << L" ADD EAX,[" << GetElemName(q,1)<<"]"<< endl;
        }
        else
        {
            text << L" PUSH EAX";
            Compile(q, data, bss, text);;
            text << L" POP EBX";
            text << L" ADD EAX,EBX"<< endl;
        }
    }
    else if (nodename== L"ADDMOV") {
        OpMov(position, L"add", data, bss, text);
    }
    else if (nodename== L"ADDROF") {
        z = ElemPos(position, 1);

        if ( GetNodeName(z) == L"VAR" ) {
            text << " lea eax, [" << GetElemName(z, 1) << "]" << std::endl;
        } else {
            text << "TODO - ADDROF" << endl;
        }
    }
    else if (nodename== L"AND") {
        z=ElemPos(position,1);
        q=ElemPos(position,2);
        Compile(z, data, bss, text);;
        if (GetNodeName(q)==L"INT")
        {
              text << L" AND EAX,"<< GetElemName(q,1)<< endl ;
        }
        else if (GetNodeName(q)==L"VAR")
        {
              text << L" AND EAX,[" << GetElemName(q,1)<<"]"<< endl;
        }
        else
        {
            text << L" PUSH EAX";
            Compile(q, data, bss, text);;
            text << L" POP EBX";
            text << L" AND EAX,EBX"<< endl;
        }
    }
    else if (nodename== L"ANDMOV") {
        OpMov(position, L"and", data, bss, text);
    }
    else if (nodename== L"ARG") {
        // todo
        text<<"TODO - ARG" << endl;
    }
    else if (nodename== L"ASIZE") {
        // todo
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
        text << " jmp " << (isInsideLoop ? CurrentLoopEndLabel : CurrentSwitchEndLabel);
    }
    else if (nodename== L"B" ) {
        while ((z=ElemPos(position,par)) >0)
        {
          Compile(z, data, bss, text);;
          par++;
        }
    }
    else if (nodename== L"CHAR") {
        // todo
        text<<"TODO - CHAR" << endl;
    }
    else if (nodename== L"CONDEXPR") {
        // todo
        text<<"TODO - CONDEXPR" << endl;
    }
    else if (nodename== L"CONTINUE") {
        text << " jmp " << CurrentLoopStartLabel << std::endl;
    }
    else if (nodename== L"DECLSTAT") {
        while ((z=ElemPos(position,par)) >0)
        {
          Compile(z, data, bss, text);;
          par++;
        }
    }
    else if (nodename== L"DEFAULT") {
        // todo
        text<<"TODO - DEFAULT" << endl;
    }
    else if (nodename== L"DIV") {
        // todo
        text<<"TODO - DIV" << endl;
    }
    else if (nodename== L"DIVMOV") {
        OpMov(position, L"idiv", data, bss, text);
    }
    else if (nodename== L"EQU") {
        z = ElemPos(position, 1);
        q = ElemPos(position, 2);

        PrintCompare(z, q, text);

        CmpToBool(L"je", text);
    }
    else if (nodename== L"EXTRN") {
        // todo
        // text<<"TODO - EXTRN" << endl;
    }
    else if (nodename== L"FHEADER") {
        elemname = GetElemName(position, 1);

        while ((z=ElemPos(position,par)) >0)
        {
            Compile(z, data, bss, text);;
            par++;
        }
    }
    else if (nodename== L"FPARAM") {
        elemname = GetElemName(position, 1);

        FunctionParameters[CurrentFunction].push_back(elemname);

        text << " %define " << elemname << " ebp + "
             << (4 + FunctionParameters[CurrentFunction].size() * 4) << endl;

    }
    else if (nodename== L"FUNCCALL") {
        //todo
        text << L" TODO - FUNC CALL " << endl;
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
             << L" RET " << endl;

        // add param length data entry
        data << elemname << "_len db " << (CalculateVarsSize(FunctionLocals[CurrentFunction])) << endl;
    }
    else if (nodename== L"GARRDEF") {
        // todo
        text<<"TODO - GARRDEF" << endl;
    }
    else if (nodename== L"GOTO") {
        elemname = GetElemName(position, 1);

        text << L" jmp label_" << elemname << endl;
    }
    else if (nodename== L"GREATEREQUTHAN") {
        z = ElemPos(position, 1);
        q = ElemPos(position, 2);

        PrintCompare(z, q, text);

        CmpToBool(L"jge", text);
    }
    else if (nodename== L"GREATERTHAN") {
        z = ElemPos(position, 1);
        q = ElemPos(position, 2);

        PrintCompare(z, q, text);

        CmpToBool(L"jg", text);
    }
    else if (nodename== L"GVARDEF") {
        elemname = GetElemName(position, 1);

        bss << elemname << " resd 1" << endl;
    }
    else if (nodename== L"WHILE") {
        isInsideLoop = true;

        int condblock = ElemPos(position, 1);
        int bodyBlock = ElemPos(position, 2);

        CurrentLoopStartLabel = NextLabel();
        CurrentLoopEndLabel = NextLabel();

        text << CurrentLoopStartLabel << L":" << std::endl;
        Compile(condblock, data, bss, text);

        text << " jz " << CurrentLoopEndLabel << std::endl;

        Compile(bodyBlock, data, bss, text);

        text << L" jmp " << CurrentLoopStartLabel << std::endl;

        text << CurrentLoopEndLabel << ":" << std::endl;

        isInsideLoop = false;
    }
    else if (nodename== L"IFELSE" || nodename == L"IF") {
        int condblock = ElemPos(position, 1);
        int ifblock = ElemPos(position, 2);
        int elseblock = ElemPos(position, 3);
        bool hasElse = nodename == L"IFELSE";

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
        // todo
        text<<"TODO - INDEX" << endl;
    }
    else if (nodename== L"INIT") {
        // Init should be handled by the outer node
        text<<"TODO - INIT" << endl;
    }
    else if (nodename== L"INT") {
        text << L" MOV EAX,"<< GetElemName(position,1)<< endl ;
    }
    else if (nodename== L"ISEQUMOV") {
        // todo OpMov
    }
    else if (nodename== L"ISGREATEREQUMOV") {
        // X >== Y
        // z je pozicija od X
        // q je pozicija od Y
        z = ElemPos(position, 1);
        q = ElemPos(position, 2);

        text << "; isgreaterequmov mov " << endl;

        if ( GetNodeName(q) == L"INT" ) {
            // X >== const
            text << " cmp [" << GetElemName(z, 1) << "], dword " << GetElemName(q, 1) << endl;;
        } else if ( GetNodeName(q) == L"VAR" ) {
            // X >== var
            text << " cmp [" << GetElemName(z, 1) << "], [" << GetElemName(q, 1) << "]" << endl;
        } else {
            // X >== expr
            Compile(q, data, bss, text);
            text << " cmp [" << GetElemName(z, 1) << "], eax" << endl;
        }

        text << " setge [" << GetElemName(z, 1) << "]" << endl;

        // rezultat izraza treba da bude vrijednost varijable
        text << " mov eax, [" << GetElemName(z, 1) << "]" << endl;
    }
    else if (nodename== L"ISGREATERMOV") {
        // X => Y
        // z je pozicija od X
        // q je pozicija od Y
        z = ElemPos(position, 1);
        q = ElemPos(position, 2);

        text << "; isgreatermov mov " << endl;

        if ( GetNodeName(q) == L"INT" ) {
            // X => const
            text << " cmp [" << GetElemName(z, 1) << "], dword " << GetElemName(q, 1) << endl;;
        } else if ( GetNodeName(q) == L"VAR" ) {
            // X => var
            text << " cmp [" << GetElemName(z, 1) << "], [" << GetElemName(q, 1) << "]" << endl;
        } else {
            // X => expr
            Compile(q, data, bss, text);
            text << " cmp [" << GetElemName(z, 1) << "], eax" << endl;
        }

        text << " setg [" << GetElemName(z, 1) << "]" << endl;

        // rezultat izraza treba da bude vrijednost varijable
        text << " mov eax, [" << GetElemName(z, 1) << "]" << endl;
    }
    else if (nodename== L"ISLESSEQUMOV") {
        // X <== Y
        // z je pozicija od X
        // q je pozicija od Y
        z = ElemPos(position, 1);
        q = ElemPos(position, 2);

        text << "; islessequmov mov " << endl;

        if ( GetNodeName(q) == L"INT" ) {
            // X <== const
            text << " cmp [" << GetElemName(z, 1) << "], dword " << GetElemName(q, 1) << endl;;
        } else if ( GetNodeName(q) == L"VAR" ) {
            // X <== var
            text << " cmp [" << GetElemName(z, 1) << "], [" << GetElemName(q, 1) << "]" << endl;
        } else {
            // X <== expr
            Compile(q, data, bss, text);
            text << " cmp [" << GetElemName(z, 1) << "], eax" << endl;
        }

        text << " setle [" << GetElemName(z, 1) << "]" << endl;

        // rezultat izraza treba da bude vrijednost varijable
        text << " mov eax, [" << GetElemName(z, 1) << "]" << endl;
    }
    else if (nodename== L"ISLESSMOV") {
        // X =< Y
        // z je pozicija od X
        // q je pozicija od Y
        z = ElemPos(position, 1);
        q = ElemPos(position, 2);

        text << "; islessmov mov " << endl;

        if ( GetNodeName(q) == L"INT" ) {
            // X =< const
            text << " cmp [" << GetElemName(z, 1) << "], dword " << GetElemName(q, 1) << endl;;
        } else if ( GetNodeName(q) == L"VAR" ) {
            // X =< var
            text << " cmp [" << GetElemName(z, 1) << "], [" << GetElemName(q, 1) << "]" << endl;
        } else {
            // X =< expr
            Compile(q, data, bss, text);
            text << " cmp [" << GetElemName(z, 1) << "], eax" << endl;
        }

        if ( GetNodeName(q) == L"VAR" ) {
            text << " setl [" << GetElemName(z, 1) << "]" << endl;
            text << " mov eax, [" << GetElemName(z, 1) << "]" << endl;
        } else {
            text << " setl [ebx]" << endl;
            text << " mov eax, [ebx]" << endl;
        }


    }
    else if (nodename== L"ISNEQUMOV") {
        z = ElemPos(position, 1);
        q = ElemPos(position, 2);

        PrintCompare(z, q, text);

        CmpToBool(L"jne", text);
    }
    else if (nodename== L"LABEL") {
        elemname = GetElemName(position, 1);

        text << L"label_" << elemname << L":" << endl;
    }
    else if (nodename== L"LESSEQUTHAN") {
        z = ElemPos(position, 1);
        q = ElemPos(position, 2);

        PrintCompare(z, q, text);

        CmpToBool(L"jle", text);
    }
    else if (nodename== L"LESSTHAN") {
        z = ElemPos(position, 1);
        q = ElemPos(position, 2);

        PrintCompare(z, q, text);

        CmpToBool(L"jl", text);
    }
    else if (nodename== L"LSHIFT") {
        // todo
        text<<"TODO - LSHIFT" << endl;
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

        FunctionLocals[CurrentFunction].push_back( LocalVariable(CurrentFunction, elemname, arrSize) );

        text << " %define " << elemname
             << " dword ebp - " << CalculateVarsSize(FunctionLocals[CurrentFunction]) << endl;


        z = ElemPos(position, 2);

        if ( GetNodeName(z) == L"INIT" ) {
            text << " mov [ebp - " << CalculateVarsSize(FunctionLocals[CurrentFunction]) << "], dword "
                 << GetElemName(z, 1) << endl;
        }
    }
    else if (nodename== L"LVARDEF") {
        elemname = GetElemName(position, 1);

        FunctionLocals[CurrentFunction].push_back( LocalVariable(CurrentFunction, elemname) );

        text << " %define " << elemname
             << " dword ebp - " << CalculateVarsSize(FunctionLocals[CurrentFunction]) << endl;


        z = ElemPos(position, 2);

        if ( GetNodeName(z) == L"INIT" ) {
            text << " mov [ebp - " << CalculateVarsSize(FunctionLocals[CurrentFunction]) << "], dword "
                 << GetElemName(z, 1) << endl;
        }
    }
    else if (nodename== L"MOD") {
        // todo
        text<<"TODO - MOD" << endl;
    }
    else if (nodename== L"MODMOV") {
        // todo
        text<<"TODO - MODMOV" << endl;
    }
    else if (nodename== L"MOV") {
        z=ElemPos(position, 1);
        q=ElemPos(position, 2);

        if (GetNodeName(z)==L"VAR")
        {
            if (GetNodeName(q)==L"INT")
            {
              text << L" MOV DWORD [" << GetElemName(z,1)<<"],"<< GetElemName(q,1)<< endl ;
            } else if (GetNodeName(q)==L"VAR")
            {
              text << L" MOV EAX, [" << GetElemName(q,1)<<"]"<< endl;
              text << L" MOV [" << GetElemName(z,1)<<"],EAX"<< endl;
            } else if (GetNodeName(q) == L"ONUMBER") {
                text << L" xor eax, eax" << std::endl;
                text << L" mov [" << GetElemName(z,1)<<"], EAX"<< endl;
            } else
            {
              Compile(q, data, bss, text);;
              text << L" MOV [" << GetElemName(z,1)<<"],EAX"<< endl;
            }
        }
        else
        {
            if (GetNodeName(q)==L"INT")
            {
              Compile(z, data, bss, text);;
              text << L" MOV DWORD [EBX],"<< GetElemName(q,1)<< endl ;
            }
            else
            if (GetNodeName(q)==L"VAR")
            {
              Compile(z, data, bss, text);;
              text << L" MOV EAX, [" << GetElemName(q,1)<<"]"<< endl;
              text << L" MOV [EBX],EAX"<< endl;
            }
            else
            {
              Compile(q, data, bss, text);;
              text << L" PUSH EAX"<< endl;
              Compile(z, data, bss, text);;
              text << L" POP EAX"<< endl;
              text << L" MOV [EBX],EAX"<< endl;
            }


        }
    }
    else if (nodename== L"MUL") {
        z=ElemPos(position,1);
        q=ElemPos(position,2);
        Compile(z, data, bss, text);;
        if (GetNodeName(q)==L"INT")
        {
              text << L" MUL EAX,"<< GetElemName(q,1)<< endl ;
        }
        else
        if (GetNodeName(q)==L"VAR")
        {
              text << L" MUL EAX,[" << GetElemName(q,1)<<"]"<< endl;
        }
        else
        {
            text << L" PUSH EAX";
            Compile(q, data, bss, text);;
            text << L" POP EBX";
            text << L" MUL EAX,EBX"<< endl;
        }
    }
    else if (nodename== L"MULTMOV") {
        OpMov(position, L"imul", data, bss, text);
    }
    else if (nodename== L"NEQU") {
        // todo
        text<<"TODO - NEQU" << endl;
    }
    else if (nodename== L"ONUMBER") {
        // ONumbers should be handles everywhere where they could occur as an operand
        SemErr(L"Unhandled zero");
    }
    else if (nodename== L"OR") {
        z=ElemPos(position,1);
        q=ElemPos(position,2);
        Compile(z, data, bss, text);;
        if (GetNodeName(q)==L"INT")
        {
              text << L" OR EAX,"<< GetElemName(q,1)<< endl ;
        }
        else
        if (GetNodeName(q)==L"VAR")
        {
              text << L" OR EAX,[" << GetElemName(q,1)<<"]"<< endl;
        }
        else
        {
            text << L" PUSH EAX";
            Compile(q, data, bss, text);;
            text << L" POP EBX";
            text << L" OR EAX,EBX"<< endl;
        }
    }
    else if (nodename== L"ORMOV") {
        OpMov(position, L"or", data, bss, text);
    }
    else if (nodename == L"POSTDEC" || nodename== L"POSTINC") {
        z = ElemPos(position, 1);

        auto zType = GetNodeName(z);

        if ( zType == L"VAR" ) {
            text << " lea ebx, [" << GetElemName(z, 1) << "]" << std::endl;
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
            text << " lea ebx, [" << GetElemName(z, 1) << "]" << std::endl;
            text << " mov eax, [ebx]" << std::endl;

            if ( nodename == L"PREDEC" ) {
                text << " dec eax" << std::endl;
            } else {
                text << " inc eax" << std::endl;
            }

            text << " mov [ebx] eax" << std::endl;
        } else {
            // izraz
            text << "todo postdec/postinc" << zType << std::endl;
        }
    }
    else if (nodename== L"PTR") {
        // todo
        text<<"TODO - PTR" << endl;
    }
    else if (nodename== L"RETURN") {
        text << " jmp " << CurrentFunction << "_end" << endl;
    }
    else if (nodename== L"RETURNPARAM") {
        z = ElemPos(position, 1);

        if ( GetNodeName(z) == L"INT" ) {
            text << L" mov eax, dword " << GetElemName(z, 1) << endl;
        } else if ( GetNodeName(z) == L"VAR" ) {
            text << L" mov eax, dword [" << GetElemName(z, 1) << L"]" << endl;
        } else {
            Compile(z, data, bss, text);
        }

        text << " jmp " << CurrentFunction << "_end" << endl;
    }
    else if (nodename== L"RSHIFT") {
        // todo
        text<<"TODO - RSHIFT" << endl;
    }
    else if (nodename== L"RSHIFTMOV") {
        OpMov(position, L"shr", data, bss, text);
    }
    else if (nodename== L"SAMEAS") {
        // todo
        text<<"TODO - SAMEAS" << endl;
    }
    else if (nodename== L"STRING") {
        // todo
        text<<"TODO - STRING" << endl;
    }
    else if (nodename== L"SUB") {
        z=ElemPos(position,1);
        q=ElemPos(position,2);
        Compile(z, data, bss, text);;
        if (GetNodeName(q)==L"INT")
        {
              text << L" SUB EAX,"<< GetElemName(q,1)<< endl ;
        }
        else
        if (GetNodeName(q)==L"VAR")
        {
              text << L" SUB EAX,[" << GetElemName(q,1)<<"]"<< endl;
        }
        else
        {
            text << L" PUSH EAX";
            Compile(q, data, bss, text);;
            text << L" POP EBX";
            text << L" SUB EAX,EBX"<< endl;
        }
    }
    else if (nodename== L"SUBMOV") {
        OpMov(position, L"sub", data, bss, text);
    }
    else if (nodename== L"SWITCH") {
        // todo
        text<<"TODO - SWITCH" << endl;
    }
    else if (nodename== L"UNEG") {
        z = ElemPos(position, 1);

        text << "; uneg";

        if ( GetNodeName(z) == L"INT" ) {
            text << " mov eax, " << GetElemName(z, 1) << endl;
        } else if ( GetNodeName(z) == L"VAR" ) {
            text << " mov eax, [" << GetElemName(z, 1) << "]" << endl;
        } else {
            Compile(z, data, bss, text);
        }

        text << " neg eax";
    }
    else if (nodename== L"UNOT") {
        z = ElemPos(position, 1);

        text << "; unot";

        if ( GetNodeName(z) == L"INT" ) {
            text << " mov eax, " << GetElemName(z, 1) << endl;
        } else if ( GetNodeName(z) == L"VAR" ) {
            text << " mov eax, [" << GetElemName(z, 1) << "]" << endl;
        } else {
            Compile(z, data, bss, text);
        }

        text << " not eax";
    }
    else if (nodename== L"UMINUS") {
        z = ElemPos(position, 1);

        auto zType = GetNodeName(z);

        if ( zType == L"VAR" ) {
            text << L" lea ebx, [" << GetElemName(z, 1) << std::endl
                 << L" mov eax, [ebx]" << std::endl
                 << L" neg eax" << std::endl
                 << L" mov [ebx], eax" << std::endl;
        } else {
            SemErr(L"Invalid operand for uminus");
        }
    }
    else if (nodename== L"UPLUS") {
        // Podrzan radi simetrije sa unarnim minusom, ali nema prave funkcije.
    }
    else if (nodename== L"VAR") {
        text << L" MOV EAX, [" << GetElemName(position,1)<<"]"<< endl;
    }
    else if (nodename== L"XOR") {
        z=ElemPos(position,1);
        q=ElemPos(position,2);
        Compile(z, data, bss, text);;
        if (GetNodeName(q)==L"INT")
        {
              text << L" XOR EAX,"<< GetElemName(q,1)<< endl ;
        }
        else
        if (GetNodeName(q)==L"VAR")
        {
              text << L" XOR EAX,[" << GetElemName(q,1)<<"]"<< endl;
        }
        else
        {
            text << L" PUSH EAX";
            Compile(q, data, bss, text);;
            text << L" POP EBX";
            text << L" XOR EAX,EBX"<< endl;
        }
    }
    else if (nodename== L"XORMOV") {
        OpMov(position, L"xor", data, bss, text);
    }
}