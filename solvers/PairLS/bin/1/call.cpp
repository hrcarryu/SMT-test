//
//  call.cpp
//  lia_ls
//
//  Created by DouglasLee on 2021/11/2.
//

#include "call.hpp"
using namespace std;
namespace call{
Call_fun::Call_fun(){
    m_ls=new lia::ls_solver(0);
}

int Call_fun::func(int argc,char *argv[]){
    int input_flag = std::atoi(argv[3]);
    m_ls->_cutoff = std::atof(argv[2]);
    string in_string;
    uint64_t num_lits;// long long int
    freopen(argv[1],"r",stdin);
    cin>>num_lits;
    m_ls->make_lits_space(num_lits);//此处需要设定lits数量
    getline(cin,in_string);
    getline(cin, in_string);
    while(in_string!="0"){
        m_ls->build_lits(in_string);//传入string到lits
        getline(cin,in_string);
    }

    //new
    int lit_num;
    cin>>lit_num;
    // m_ls->make_lits_space(num_lits+lit_num);
    vector<string>lit_arr;
    vector<string>lit_weight(lit_num+1);//new add
    //std::cout<<lit_weight.size();
    int curpos=0;
    for(int i=0;i<lit_num;i++)
    {
        lit_arr.emplace_back("");
    }
    getline(cin,in_string);
    getline(cin,in_string);//this
    lit_weight[curpos++]=in_string;
    for(int cur=0;cur<lit_num;cur++)
    {
        getline(cin,in_string);
        while(in_string[0]!='s')
        {
            lit_arr[cur].append(in_string);
            getline(cin,in_string);//this
            if(in_string[0]=='s')
            {
                lit_weight[curpos++]=in_string;
            }
        }
    }
    //std::cout<<curpos<<std::endl;
    m_ls->build_softlits(lit_arr,lit_num);
    m_ls->build_soft_weight(lit_weight);
    //new

    int size;
    cin>>size;
    std::vector<std::vector<int> > vec;
    vec.resize(size);
    int size_now=0;
    while(size_now<size){
        cin>>in_string;
        if(in_string=="(")continue;
        else if(in_string==")"){size_now++;}
        else{vec[size_now].push_back(atoi(in_string.c_str()));}
    }
    m_ls->build_instance(vec);//传入一个vector<vector<int> >
    getline(cin,in_string);
    //std::cout<<in_string<<std::endl;
    for(int i=0;i<4;i++)
    {
        getline(cin,in_string);
        if(i==2)  m_ls->initial_bound(in_string);
    }
//    m_ls->print_formula();
//    m_ls->print_formula_pbs();
    m_ls->local_search();
    std::cout<<std::endl;
    if (input_flag == 1) m_ls->print_solution();
    return 0;
}
}
using namespace call;
int main(int argc,char *argv[]){
    Call_fun c;
    c.func(argc,argv);
    return 0;
}
