

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





std::map< std::wstring, std::vector< std::wstring > > FunctionParameters;
std::map< std::wstring, std::vector< std::wstring > > FunctionLocals;

std::wstring CurrentFunction;

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
        // X += Y
        // z je pozicija od X
        // q je pozicija od Y
        z = ElemPos(position, 1);
        q = ElemPos(position, 2);

        text << "; add mov " << endl;

        if ( GetNodeName(q) == L"INT" ) {
            // X += const
            text << " add [" << GetElemName(z, 1) << "], dword " << GetElemName(q, 1) << endl;;
        } else if ( GetNodeName(q) == L"VAR" ) {
            // X += var
            text << " add [" << GetElemName(z, 1) << "], [" << GetElemName(q, 1) << "]" << endl;
        } else {
            // X += expr
            Compile(q, data, bss, text);
            text << " add [" << GetElemName(z, 1) << "], eax" << endl;
        }

        // rezultat izraza treba da bude vrijednost varijable
        text << " mov eax, [" << GetElemName(z, 1) << "]" << endl;
    }
    else if (nodename== L"ADDROF") {
        // todo
        text<<"TODO - ADDROF" << endl;
    }
    else if (nodename== L"AND") {
        z=ElemPos(position,1);
        q=ElemPos(position,2);
        Compile(z, data, bss, text);;
        if (GetNodeName(q)==L"INT")
        {
              text << L" AND EAX,"<< GetElemName(q,1)<< endl ;
        }
        else
        if (GetNodeName(q)==L"VAR")
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
        // X &= Y
        // z je pozicija od X
        // q je pozicija od Y
        z = ElemPos(position, 1);
        q = ElemPos(position, 2);

        text << "; and mov " << endl;

        if ( GetNodeName(q) == L"INT" ) {
            // X &= const
            text << " and [" << GetElemName(z, 1) << "], dword " << GetElemName(q, 1) << endl;;
        } else if ( GetNodeName(q) == L"VAR" ) {
            // X &= var
            text << " and [" << GetElemName(z, 1) << "], [" << GetElemName(q, 1) << "]" << endl;
        } else {
            // X &= expr
            Compile(q, data, bss, text);
            text << " and [" << GetElemName(z, 1) << "], eax" << endl;
        }

        // rezultat izraza treba da bude vrijednost varijable
        text << " mov eax, [" << GetElemName(z, 1) << "]" << endl;
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
          Compile(z, data, bss, text);;
          par++;
        }
    }
    else if (nodename== L"BREAK") {
        // todo
        text<<"TODO - BREAK" << endl;
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
        // todo
        text<<"TODO - CONTINUE" << endl;
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
        // X /= Y
        // z je pozicija od X
        // q je pozicija od Y
        z = ElemPos(position, 1);
        q = ElemPos(position, 2);

        text << "; idiv mov " << endl;

        if ( GetNodeName(q) == L"INT" ) {
            // X /= const
            text << " idiv [" << GetElemName(z, 1) << "], dword " << GetElemName(q, 1) << endl;;
        } else if ( GetNodeName(q) == L"VAR" ) {
            // X /= var
            text << " idiv [" << GetElemName(z, 1) << "], [" << GetElemName(q, 1) << "]" << endl;
        } else {
            // X /= expr
            Compile(q, data, bss, text);
            text << " idiv [" << GetElemName(z, 1) << "], eax" << endl;
        }

        // rezultat izraza treba da bude vrijednost varijable
        text << " mov eax, [" << GetElemName(z, 1) << "]" << endl;
    }
    else if (nodename== L"EQU") {
        // todo
        text<<"TODO - EQU" << endl;
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
        data << elemname << "_len db " << (FunctionLocals[CurrentFunction].size() * 4) << endl;
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
        // todo
        text<<"TODO - GREATEREQUTHAN" << endl;
    }
    else if (nodename== L"GREATERTHAN") {
        // todo
        text<<"TODO - GREATERTHAN" << endl;
    }
    else if (nodename== L"GVARDEF") {
        elemname = GetElemName(position, 1);

        bss << elemname << " resd 1" << endl;
    }
    else if (nodename== L"IFELSE") {
        // todo
        text<<"TODO - IFELSE" << endl;
    }
    else if (nodename== L"IF") {
        // todo
        text<<"TODO - IF" << endl;
    }
    else if (nodename== L"INDEX") {
        // todo
        text<<"TODO - INDEX" << endl;
    }
    else if (nodename== L"INIT") {
        // todo
        text<<"TODO - INIT" << endl;
    }
    else if (nodename== L"INT") {
        text << L" MOV EAX,"<< GetElemName(position,1)<< endl ;
    }
    else if (nodename== L"ISEQUMOV") {
        // X === Y
        // z je pozicija od X
        // q je pozicija od Y
        z = ElemPos(position, 1);
        q = ElemPos(position, 2);

        text << "; isequ mov " << endl;

        if ( GetNodeName(q) == L"INT" ) {
            // X === const
            text << " cmp [" << GetElemName(z, 1) << "], dword " << GetElemName(q, 1) << endl;;
        } else if ( GetNodeName(q) == L"VAR" ) {
            // X === var
            text << " cmp [" << GetElemName(z, 1) << "], [" << GetElemName(q, 1) << "]" << endl;
        } else {
            // X === expr
            Compile(q, data, bss, text);
            text << " cmp [" << GetElemName(z, 1) << "], eax" << endl;
        }

        text << " sete [" << GetElemName(z, 1) << "]" << endl;

        // rezultat izraza treba da bude vrijednost varijable
        text << " mov eax, [" << GetElemName(z, 1) << "]" << endl;
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
        // X =!= Y
        // z je pozicija od X
        // q je pozicija od Y
        z = ElemPos(position, 1);
        q = ElemPos(position, 2);

        text << "; isnequ mov " << endl;

        if ( GetNodeName(q) == L"INT" ) {
            // X =!= const
            text << " cmp [" << GetElemName(z, 1) << "], dword " << GetElemName(q, 1) << endl;;
        } else if ( GetNodeName(q) == L"VAR" ) {
            // X =!= var
            text << " cmp [" << GetElemName(z, 1) << "], [" << GetElemName(q, 1) << "]" << endl;
        } else {
            // X =!= expr
            Compile(q, data, bss, text);
            text << " cmp [" << GetElemName(z, 1) << "], eax" << endl;
        }

        text << " setne [" << GetElemName(z, 1) << "]" << endl;

        // rezultat izraza treba da bude vrijednost varijable
        text << " mov eax, [" << GetElemName(z, 1) << "]" << endl;
    }
    else if (nodename== L"LABEL") {
        elemname = GetElemName(position, 1);

        text << L"label_" << elemname << L":" << endl;
    }
    else if (nodename== L"LARRDEF") {
        // todo
        text<<"TODO - LARRDEF" << endl;
    }
    else if (nodename== L"LESSEQUTHAN") {
        // todo
        text<<"TODO - LESSEQUTHAN" << endl;
    }
    else if (nodename== L"LESSTHAN") {
        // todo
        text<<"TODO - LESSTHAN" << endl;
    }
    else if (nodename== L"LSHIFT") {
        // todo
        text<<"TODO - LSHIFT" << endl;
    }
    else if (nodename== L"LSHIFTMOV") {
        // X <<= Y
        // z je pozicija od X
        // q je pozicija od Y
        z = ElemPos(position, 1);
        q = ElemPos(position, 2);

        text << "; shl mov " << endl;

        if ( GetNodeName(q) == L"INT" ) {
            // X <<= const
            text << " shl [" << GetElemName(z, 1) << "], dword " << GetElemName(q, 1) << endl;;
        } else if ( GetNodeName(q) == L"VAR" ) {
            // X <<= var
            text << " shl [" << GetElemName(z, 1) << "], [" << GetElemName(q, 1) << "]" << endl;
        } else {
            // X <<= expr
            Compile(q, data, bss, text);
            text << " shl [" << GetElemName(z, 1) << "], eax" << endl;
        }

        // rezultat izraza treba da bude vrijednost varijable
        text << " mov eax, [" << GetElemName(z, 1) << "]" << endl;
    }
    else if (nodename== L"LVARDEF") {
        elemname = GetElemName(position, 1);

        text << " %define " << elemname
             << " dword ebp - " << (FunctionLocals[CurrentFunction].size() * 4) << endl;

        FunctionLocals[CurrentFunction].push_back( elemname );

        z = ElemPos(position, 2);

        if ( GetNodeName(z) == L"INIT" ) {
            Compile(z, data, bss, text);
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
        z=ElemPos(position,1);
        q=ElemPos(position,2);
        if (GetNodeName(z)==L"VAR")
        {
            if (GetNodeName(q)==L"INT")
            {
              text << L" MOV DWORD [" << GetElemName(z,1)<<"],"<< GetElemName(q,1)<< endl ;
            }
            else
            if (GetNodeName(q)==L"VAR")
            {
              text << L" MOV EAX, [" << GetElemName(q,1)<<"]"<< endl;
              text << L" MOV [" << GetElemName(z,1)<<"],EAX"<< endl;
            }
            else
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
        // X *= Y
        // z je pozicija od X
        // q je pozicija od Y
        z = ElemPos(position, 1);
        q = ElemPos(position, 2);

        text << "; imul mov " << endl;

        if ( GetNodeName(q) == L"INT" ) {
            // X *= const
            text << " imul [" << GetElemName(z, 1) << "], dword " << GetElemName(q, 1) << endl;;
        } else if ( GetNodeName(q) == L"VAR" ) {
            // X *= var
            text << " imul [" << GetElemName(z, 1) << "], [" << GetElemName(q, 1) << "]" << endl;
        } else {
            // X *= expr
            Compile(q, data, bss, text);
            text << " imul [" << GetElemName(z, 1) << "], eax" << endl;
        }

        // rezultat izraza treba da bude vrijednost varijable
        text << " mov eax, [" << GetElemName(z, 1) << "]" << endl;
    }
    else if (nodename== L"NEQU") {
        // todo
        text<<"TODO - NEQU" << endl;
    }
    else if (nodename== L"ONUMBER") {
        // todo
        text<<"TODO - ONUMBER" << endl;
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
        // X |= Y
        // z je pozicija od X
        // q je pozicija od Y
        z = ElemPos(position, 1);
        q = ElemPos(position, 2);

        text << "; or mov " << endl;

        if ( GetNodeName(q) == L"INT" ) {
            // X |= const
            text << " or [" << GetElemName(z, 1) << "], dword " << GetElemName(q, 1) << endl;;
        } else if ( GetNodeName(q) == L"VAR" ) {
            // X |= var
            text << " or [" << GetElemName(z, 1) << "], [" << GetElemName(q, 1) << "]" << endl;
        } else {
            // X |= expr
            Compile(q, data, bss, text);
            text << " or [" << GetElemName(z, 1) << "], eax" << endl;
        }

        // rezultat izraza treba da bude vrijednost varijable
        text << " mov eax, [" << GetElemName(z, 1) << "]" << endl;
    }
    else if (nodename== L"POSTDEC") {
        // todo
        text<<"TODO - POSTDEC" << endl;
    }
    else if (nodename== L"POSTINC") {
        // todo
        text<<"TODO - POSTINC" << endl;
    }
    else if (nodename== L"PREDEC") {
        // todo
        text<<"TODO - PREDEC" << endl;
    }
    else if (nodename== L"PREINC") {
        // todo
        text<<"TODO - PREINC" << endl;
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
        // X >>= Y
        // z je pozicija od X
        // q je pozicija od Y
        z = ElemPos(position, 1);
        q = ElemPos(position, 2);

        text << "; shr mov " << endl;

        if ( GetNodeName(q) == L"INT" ) {
            // X >>= const
            text << " shr [" << GetElemName(z, 1) << "], dword " << GetElemName(q, 1) << endl;;
        } else if ( GetNodeName(q) == L"VAR" ) {
            // X >>= var
            text << " shr [" << GetElemName(z, 1) << "], [" << GetElemName(q, 1) << "]" << endl;
        } else {
            // X >>= expr
            Compile(q, data, bss, text);
            text << " shr [" << GetElemName(z, 1) << "], eax" << endl;
        }

        // rezultat izraza treba da bude vrijednost varijable
        text << " mov eax, [" << GetElemName(z, 1) << "]" << endl;
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
        // X -= Y
        // z je pozicija od X
        // q je pozicija od Y
        z = ElemPos(position, 1);
        q = ElemPos(position, 2);

        text << "; sub mov " << endl;

        if ( GetNodeName(q) == L"INT" ) {
            // X -= const
            text << " sub [" << GetElemName(z, 1) << "], dword " << GetElemName(q, 1) << endl;;
        } else if ( GetNodeName(q) == L"VAR" ) {
            // X -= var
            text << " sub [" << GetElemName(z, 1) << "], [" << GetElemName(q, 1) << "]" << endl;
        } else {
            // X -= expr
            Compile(q, data, bss, text);
            text << " sub [" << GetElemName(z, 1) << "], eax" << endl;
        }

        // rezultat izraza treba da bude vrijednost varijable
        text << " mov eax, [" << GetElemName(z, 1) << "]" << endl;
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
        // todo
        text<<"TODO - UMINUS" << endl;
    }
    else if (nodename== L"UPLUS") {
        // Podrzan radi simetrije sa unarnim minusom, ali nema prave funkcije.
    }
    else if (nodename== L"VAR") {
        text << L" MOV EAX, [" << GetElemName(position,1)<<"]"<< endl;
    }
    else if (nodename== L"WHILE") {
        // todo
        text<<"TODO - WHILE" << endl;
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
    else if (nodename== L"XORMOV ") {
        // X ^= Y
        // z je pozicija od X
        // q je pozicija od Y
        z = ElemPos(position, 1);
        q = ElemPos(position, 2);

        text << "; xor mov " << endl;

        if ( GetNodeName(q) == L"INT" ) {
            // X ^= const
            text << " xor [" << GetElemName(z, 1) << "], dword " << GetElemName(q, 1) << endl;;
        } else if ( GetNodeName(q) == L"VAR" ) {
            // X ^= var
            text << " xor [" << GetElemName(z, 1) << "], [" << GetElemName(q, 1) << "]" << endl;
        } else {
            // X ^= expr
            Compile(q, data, bss, text);
            text << " xor [" << GetElemName(z, 1) << "], eax" << endl;
        }

        // rezultat izraza treba da bude vrijednost varijable
        text << " mov eax, [" << GetElemName(z, 1) << "]" << endl;
    }
}