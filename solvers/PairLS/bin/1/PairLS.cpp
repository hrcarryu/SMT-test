#include "lia_ls.h"
#include <sstream>
namespace lia
{
    // input transformation
    void ls_solver::split_string(std::string &in_string, std::vector<std::string> &str_vec, std::string pattern = " ")
    {
        std::string::size_type pos; // 经常用于vector的下标 size_type npos
        // size_t pos;
        in_string += pattern;
        size_t size = in_string.size(); // 大小和数组下标用size_t;
        for (size_t i = 0; i < size; i++)
        {
            pos = in_string.find(pattern, i);
            if (pos < size)
            {
                std::string s = in_string.substr(i, pos - i);
                str_vec.push_back(s);
                i = pos + pattern.size() - 1;
            }
        }
    }

    void ls_solver::build_soft_weight(std::vector<std::string> lit_weight)
    {
        is_weighted = 0;
        h_inc = 1;
        for (int i = 0; i < lit_weight.size() - 1; i++)
        {
            std::vector<std::string> vec;
            split_string(lit_weight[i], vec);
            soft_clause_weight.push_back(stoi(vec[vec.size() - 1]));
            // std::cout<<vec[vec.size()-1]<<std::endl;
        }
        for (int i : soft_clause_weight)
        {
            if (i != 1)
            {
                is_weighted = 1;
                h_inc = 3;
            }
        }
        // std::cout<<is_weighted<<std::endl;
    }

    void ls_solver::build_softlits(std::vector<std::string> lit_arr, int lit_num)
    {
        std::vector<std::string> lit_final;
        for (int i = 0; i < lit_num; i++)
        {
            lit_final.emplace_back("");
        }
        for (int j = 0; j < lit_num; j++)
        {
            std::vector<std::string> vec;
            split_string(lit_arr[j], vec);
            for (int i = 0; i < vec.size(); i++)
            {
                if (vec[i] != "")
                {
                    lit_final[j].append(vec[i]);
                    if (i != vec.size() - 1)
                        lit_final[j].append(" ");
                }
            }
        }
        // for(int i=0;i<lit_num;i++)
        // {
        //     cout<<lit_final[i]<<endl;
        // }

        lit testlit[lit_num];
        int new_lit_num = 0;
        for (int i = 0; i < lit_num; i++)
        {
            std::vector<std::string> vec;
            split_string(lit_final[i], vec);
            lit *l = &(testlit[i]);
            int idx;
            int islessthan = 1;
            int isnot = 0;
            // if(i==172) std::cout<<" size :"<<vec.size()<<std::endl;
            if (vec.size() <= 5)
            {
                // read a >= b
                if (vec[0] == "(not")
                {
                    isnot = 1;
                    if (vec[1] == "(<=")
                    {
                        l->pos_coff.push_back(1);
                        l->pos_coff_var_idx.push_back((int)transfer_name_to_tmp_var1(vec[2], 1));
                        if (vec[3] == "(-")
                        {
                            // l->key = stoi(delete_pa(vec[4]));
                            l->key = atoll(delete_pa(vec[4]).c_str());
                        }
                        else
                        {
                            // l->key = -stoi(delete_pa(vec[3]));
                            l->key = -atoll(delete_pa(vec[3]).c_str());
                        }
                    }
                    else
                    {
                        l->neg_coff.push_back(1);
                        l->neg_coff_var_idx.push_back((int)transfer_name_to_tmp_var1(vec[2], 1));
                        if (vec[3] == "(-")
                        {
                            // l->key = -stoi(delete_pa(vec[4]));
                            l->key = -atoll(delete_pa(vec[4]).c_str());
                        }
                        else
                        {
                            // l->key = stoi(delete_pa(vec[3]));
                            l->key = atoll(delete_pa(vec[3]).c_str());
                        }
                    }
                }
                else
                {
                    isnot = 0;
                    if (vec[0] == "(<=")
                    {
                        l->pos_coff.push_back(1);
                        l->pos_coff_var_idx.push_back((int)transfer_name_to_tmp_var1(vec[1], 1));
                        if (vec[2] == "(-")
                        {
                            // l->key = stoi(delete_pa(vec[3]));
                            l->key = atoll(delete_pa(vec[3]).c_str());
                        }
                        else
                        {
                            // l->key = -stoi(delete_pa(vec[2]));
                            l->key = -atoll(delete_pa(vec[2]).c_str());
                        }
                    }
                    else
                    {
                        l->neg_coff.push_back(1);
                        l->neg_coff_var_idx.push_back((int)transfer_name_to_tmp_var1(vec[1], 1));
                        if (vec[2] == "(-")
                        {
                            // l->key = -stoi(delete_pa(vec[3]));
                            l->key = -atoll(delete_pa(vec[3]).c_str());
                        }
                        else
                        {
                            // l->key = stoi(delete_pa(vec[2]));
                            l->key = atoll(delete_pa(vec[2]).c_str());
                        }
                    }
                }
            }
            else
            {
                if (vec[0] == "(not")
                {
                    idx = 3;
                    isnot = 1;
                    if (vec[1] == "(<=")
                        islessthan = 1;
                    else
                        islessthan = 0;
                }
                else
                {
                    idx = 2;
                    isnot = 0;
                    if (vec[0] == "(<=")
                        islessthan = 1;
                    else
                        islessthan = 0;
                }
                if (vec[idx] == "(*")
                {
                    for (; idx < vec.size();)
                    {
                        if (vec[idx] == "(*" && vec[idx + 1] == "(-")
                        {
                            std::string num = "";
                            num.append(1, vec[idx + 2][0]);
                            __int128_t coff = -atoll(num.c_str());
                            std::string var = delete_pa(vec[idx + 3]);
                            if (coff < 0)
                            {
                                l->neg_coff.push_back(-coff);
                                l->neg_coff_var_idx.push_back((int)transfer_name_to_tmp_var1(var, 1));
                            }
                            else
                            {
                                l->pos_coff.push_back(coff);
                                l->pos_coff_var_idx.push_back((int)transfer_name_to_tmp_var1(var, 1));
                            }
                            idx += 4;
                        }
                        else
                        {
                            // std::cout<<i<<std::endl;
                            __int128_t k;
                            if (vec[idx] == "(-")
                                // k = -stoi(delete_pa(vec[idx + 1]));
                                k = -atoll(delete_pa(vec[idx + 1]).c_str());
                            else
                            {
                                // k = stoi(delete_pa(vec[idx]));
                                k = atoll(delete_pa(vec[idx]).c_str());
                            }
                            if (islessthan == 1)
                            {
                                // l->key = -k;
                                l->key = 1 - (k + 1);
                            }
                            else
                            {
                                l->key = k;
                            }
                            break;
                        }
                    }
                }
                else
                {
                    // to be added
                    for (; idx < vec.size(); idx++)
                    {
                        std::string var = vec[idx];
                        if (is_pa(var) == 0 || var[0] == 'x')
                        {
                            if (is_pa(var) == 1)
                                var = delete_pa(var);
                            l->pos_coff.push_back(1);
                            l->pos_coff_var_idx.push_back((int)transfer_name_to_tmp_var1(var, 1));
                        }
                        else
                        {
                            __int128_t k;
                            k = atoll(delete_pa(vec[idx]).c_str());
                            if (islessthan == 1)
                            {
                                // l->key = -k;
                                l->key = 1 - (k + 1);
                            }
                            else
                            {
                                l->key = k;
                            }
                            break;
                        }
                    }
                }
                // for (; idx < vec.size();)
                // {
                //     if (vec[idx] == "(*" && vec[idx + 1] == "(-")
                //     {
                //         std::string num = "";
                //         num.append(1, vec[idx + 2][0]);
                //         __int128_t coff = -atoll(num.c_str());
                //         std::string var = delete_pa(vec[idx + 3]);
                //         if (coff < 0)
                //         {
                //             l->neg_coff.push_back(-coff);
                //             l->neg_coff_var_idx.push_back((int)transfer_name_to_tmp_var1(var,1));
                //         }
                //         else
                //         {
                //             l->pos_coff.push_back(coff);
                //             l->pos_coff_var_idx.push_back((int)transfer_name_to_tmp_var1(var,1));
                //         }
                //         idx += 4;
                //     }
                //     else
                //     {
                //         //std::cout<<i<<std::endl;
                //         __int128_t k;
                //         if (vec[idx] == "(-")
                //             //k = -stoi(delete_pa(vec[idx + 1]));
                //             k=-atoll(delete_pa(vec[idx + 1]).c_str());
                //         else
                //         {
                //             //k = stoi(delete_pa(vec[idx]));
                //             k=atoll(delete_pa(vec[idx]).c_str());
                //         }
                //         if (islessthan == 1)
                //         {
                //             //l->key = -k;
                //             l->key= 1-(k+1);
                //         }
                //         else
                //         {
                //             l->key = k;
                //         }
                //         break;
                //     }
                // }
                if (islessthan == 0)
                {
                    // vector<string> tmp_coff_var_idx = l->pos_coff_var_idx;
                    // vector<int> tmp_coff = l->pos_coff;
                    // l->pos_coff_var_idx = l->neg_coff_var_idx;
                    // l->pos_coff = l->neg_coff;
                    // l->neg_coff_var_idx = tmp_coff_var_idx;
                    // cout<<i<<endl;
                    // l->neg_coff = tmp_coff;
                    std::vector<int> tmp_coff_var_idx = testlit[i].pos_coff_var_idx;
                    std::vector<__int128_t> tmp_coff = testlit[i].pos_coff;
                    testlit[i].pos_coff_var_idx = testlit[i].neg_coff_var_idx;
                    testlit[i].pos_coff = testlit[i].neg_coff;
                    testlit[i].neg_coff_var_idx = tmp_coff_var_idx;
                    testlit[i].neg_coff = tmp_coff;
                }
            }
            int isfind = 0;
            for (int j = 0; j < _num_lits; j++)
            {
                if (testlit[i] == _lits[j])
                {
                    isfind = 1;
                    // std::cout<<"find"<<i<<"  "<<j<<std::endl;
                    if (isnot == 1)
                    {
                        soft_clause_lit.push_back(-_lits[j].lits_index);
                    }
                    else
                    {
                        soft_clause_lit.push_back(_lits[j].lits_index);
                    }
                    break;
                }
            }
            if (isfind == 0)
            {
                // std::cout<<i<<std::endl;
                new_lit_num++;
                testlit[i].is_lia_lit = true;
                testlit[i].lits_index = _num_lits - 1 + new_lit_num;
                //_lits.push_back(testlit[i]);
                // lit *l=&_lits[_num_lits-1+new_lit_num];
                // l->key=testlit[i].key;
                // l->pos_coff=testlit[i].pos_coff;
                // l->pos_coff_var_idx=testlit[i].pos_coff_var_idx;
                // l->neg_coff=testlit[i].neg_coff;
                // l->neg_coff_var_idx=testlit[i].neg_coff_var_idx;
                // l->is_lia_lit=testlit[i].is_lia_lit;
                if (new_lit_num <= _additional_len)
                {
                    _lits[_num_lits - 1 + new_lit_num] = testlit[i]; // to be test
                }
                else
                {
                    _lits.push_back(testlit[i]);
                }
                if (isnot == 1)
                    soft_clause_lit.push_back(-(_num_lits - 1 + new_lit_num)); // to
                else
                    soft_clause_lit.push_back((_num_lits - 1 + new_lit_num));
            }
        }
        _num_lits += new_lit_num;
        // for(int i=0;i<soft_clause_lit.size();i++)
        // {
        //     std::cout<<soft_clause_lit[i]<<std::endl;
        // }

        // for(int i=0;i<_num_lits;i++)
        //    //print_literal(_lits[i]);
    }

    std::string ls_solver::delete_pa(std::string org)
    {
        int last = org.size();
        std::string now;
        for (int i = 0; i < org.size(); i++)
        {
            if (org[i] == ')')
            {
                last = i;
                break;
            }
        }
        now = org.substr(0, last);
        return now;
    }

    int ls_solver::is_pa(std::string org)
    {
        int flag = 0;
        for (int i = 0; i < org.size(); i++)
        {
            if (org[i] == ')' || org[i] == '(')
            {
                flag = 1;
                break;
            }
        }
        return flag;
    }
    
    void ls_solver::build_lits(std::string &in_string)
    {
        std::vector<std::string> vec;
        split_string(in_string, vec);
        if (vec[0] == "0")
        {
            _lits[0].lits_index = 0;
            return;
        } // true literal
        int lit_index = std::atoi(vec[0].c_str());
        lit *l = &(_lits[lit_index]);
        if (vec[1] == "or" || vec[1] == "if")
        {
            l->delta = transfer_name_to_resolution_var(vec[2], false);
            l->key = 1;
            l->is_lia_lit = false;
            l->lits_index = lit_index;
            _num_opt++;
            return;
        } // or term in the form: 1 or newvar_2
        if (vec.size() > 2)
        {
            l->is_lia_lit = true;
            if (vec.size() > 6)
            {
                l->lits_index = std::atoi(vec[0].c_str());
                int idx = 5;
                if (vec[2] == "=" && vec[3] != "(")
                {
                    idx++;
                    l->key = -std::atoll(vec[3].c_str());
                }
                l->is_equal = (vec[2] == "=");
                for (; idx < vec.size(); idx++)
                {
                    if (vec[idx] == ")")
                    {
                        break;
                    }
                    if (vec[idx] == "(")
                    {
                        idx += 2;
                        __int128_t coff = std::atoll(vec[idx].c_str());
                        if (coff > 0)
                        {
                            l->pos_coff.push_back(coff);
                            l->pos_coff_var_idx.push_back((int)transfer_name_to_tmp_var(vec[++idx]));
                        }
                        else
                        {
                            l->neg_coff.push_back(-coff);
                            l->neg_coff_var_idx.push_back((int)transfer_name_to_tmp_var(vec[++idx]));
                        }
                        idx++;
                    }
                    else
                    {
                        l->pos_coff.push_back(1);
                        l->pos_coff_var_idx.push_back((int)transfer_name_to_tmp_var(vec[idx]));
                    }
                    _num_opt += l->pos_coff.size(); // 有问题
                    _num_opt += l->neg_coff.size();
                }
                if (vec[2] != "=" || vec[3] == "(")
                {
                    l->key = -std::atoll(vec[++idx].c_str()); //?
                    // std::cout << -std::atoll(vec[idx].c_str())<<std::endl;
                }
                if (vec[2] == ">=")
                {
                    l->key++;
                    invert_lit(*l);
                }
            } //( <= ( + x1 ( * -1 x2 ) x7 ( * -1 x8 ) ) 0 )
            else
            {
                l->lits_index = std::atoi(vec[0].c_str());
                __int128_t bound = std::atoll(vec[4].c_str());
                uint64_t var_idx = transfer_name_to_tmp_var(vec[3]);
                if (vec[2] == ">=")
                {
                    l->key = bound;
                    l->neg_coff.push_back(1);
                    l->neg_coff_var_idx.push_back((int)var_idx);
                }
                else
                {
                    l->key = -bound;
                    l->pos_coff.push_back(1);
                    l->pos_coff_var_idx.push_back((int)var_idx);
                }
                l->is_equal = (vec[2] == "=");
                _num_opt += 2;
            } //( >= x 0 )

        } // lia lit
        else
        {
            l->delta = transfer_name_to_resolution_var(vec[1], false);
            l->key = 1;
            l->is_lia_lit = false;
            l->lits_index = lit_index;
            _num_opt++;
        } // boolean lit
    }

    void ls_solver::build_instance(std::vector<std::vector<int>> &clause_vec)
    {
        // std::cout <<"bool var size:" << _resolution_vars.size() << std::endl;
        // std::cout <<"int var size:" <<  _tmp_vars.size() << std::endl;
        for (int clause_idx = 0; clause_idx < clause_vec.size(); clause_idx++)
        {
            // std:: cout<<clause_vec[clause_idx].size()<<std::endl;
            if (clause_vec[clause_idx].size() == 1)
            {
                lit *l = &(_lits[std::abs(clause_vec[clause_idx][0])]);
                if (l->is_equal || !l->is_lia_lit)
                {
                    continue;
                } // equal lit is not bound lit
                if (l->pos_coff.size() == 0 && l->neg_coff.size() == 1 && l->neg_coff[0] == 1)
                {
                    if (clause_vec[clause_idx][0] > 0 && l->key > _tmp_vars[l->neg_coff_var_idx[0]].low_bound)
                    {
                        _tmp_vars[l->neg_coff_var_idx[0]].low_bound = l->key;
                    }
                    else if (clause_vec[clause_idx][0] < 0 && (l->key - 1) < _tmp_vars[l->neg_coff_var_idx[0]].upper_bound)
                    {
                        _tmp_vars[l->neg_coff_var_idx[0]].upper_bound = (l->key - 1);
                    }
                    _bound_lits.push_back(l->lits_index);
                    _bound_clauses.push_back(clause_vec[clause_idx][0]);
                    // l->lits_index = 0;
                    //  if (clause_vec[clause_idx][0] < 0)
                    //  {
                    //      clause_vec[clause_idx][0] = -clause_vec[clause_idx][0];//?
                    //  }
                }
                else if (l->pos_coff.size() == 1 && l->neg_coff.size() == 0 && l->pos_coff[0] == 1)
                {
                    if (clause_vec[clause_idx][0] > 0 && (-l->key) < _tmp_vars[l->pos_coff_var_idx[0]].upper_bound)
                    {
                        _tmp_vars[l->pos_coff_var_idx[0]].upper_bound = -l->key;
                    }
                    else if (clause_vec[clause_idx][0] < 0 && (1 - l->key) > _tmp_vars[l->pos_coff_var_idx[0]].low_bound)
                    {
                        _tmp_vars[l->pos_coff_var_idx[0]].low_bound = (1 - l->key);
                    }
                    _bound_lits.push_back(l->lits_index);
                    _bound_clauses.push_back(clause_vec[clause_idx][0]);
                    // l->lits_index = 0;
                    //  if (clause_vec[clause_idx][0] < 0)
                    //  {
                    //      clause_vec[clause_idx][0] = -clause_vec[clause_idx][0];
                    //  }
                }
            }
        }              // dont know
        reduce_vars(); //?
        _clauses.resize(clause_vec.size());
        _num_clauses = 0;
        for (auto clause_curr : clause_vec)
        {
            bool is_tautology = false;
            for (auto l_idx : clause_curr)
            {
                if (_lits[std::abs(l_idx)].lits_index == 0 /*&&l_idx>=0*/)
                {
                    is_tautology = true;
                    break;
                }
                if (_lits[std::abs(l_idx)].lits_index == -2 && l_idx > 0 || _lits[std::abs(l_idx)].lits_index == -1 && l_idx < 0)
                {
                    is_tautology = true;
                    break;
                }
            }
            if (is_tautology)
            {
                continue;
            }
            for (auto l_idx : clause_curr)
            {
                if (_lits[std::abs(l_idx)].lits_index == -2 && l_idx < 0 || _lits[std::abs(l_idx)].lits_index == -1 && l_idx > 0)
                {
                    continue;
                }
                _clauses[_num_clauses].literals.push_back(l_idx);
                lit *l = &(_lits[std::abs(l_idx)]);
                if (l->lits_index == 0)
                {
                    continue;
                }
                if (!l->is_lia_lit)
                {
                    _resolution_vars[l->delta].clause_idxs.push_back(_num_clauses);
                }
            }
            _num_clauses++;
        }
        _clauses.resize(_num_clauses);
        // now the vars are all in the resolution vars
        unit_prop();
        resolution();
        unit_prop();
        reduce_clause();
        // print_formula();
        best_found_cost = (int)_num_clauses;
        make_space();
        set_pre_value();
        int average_lits_num = 0;
        for (int var_idx = 0; var_idx < _num_vars; var_idx++)
        {
            average_lits_num += _vars[var_idx].literals.size();
        }
        average_lits_num /= _num_vars + 1;
        // std::cout<<"average lit num:\n"<<average_lits_num<<"\n";
        use_swap_from_from_small_weight = (average_lits_num < 50);
    }

    uint64_t ls_solver::transfer_name_to_reduced_var(std::string &name, bool is_lia)
    {
        if (name2var.find(name) == name2var.end())
        {
            name2var[name] = _vars.size();
            variable var;
            var.var_name = name;
            var.is_lia = is_lia;
            _vars.push_back(var);
            if (is_lia)
            {
                lia_var_vec.push_back((int)_vars.size() - 1);
            }
            else
            {
                bool_var_vec.push_back((int)_vars.size() - 1);
            }
            return _vars.size() - 1;
        }
        else
            return name2var[name];
    }

    uint64_t ls_solver::transfer_name_to_resolution_var(std::string &name, bool is_lia)
    {
        if (name2resolution_var.find(name) == name2resolution_var.end())
        {
            name2resolution_var[name] = _resolution_vars.size();
            variable var;
            var.clause_idxs.reserve(64);
            var.literals.reserve(64);
            var.literal_clause.reserve(64);
            var.literal_coff.reserve(64);
            var.var_name = name;
            var.is_lia = is_lia;
            _resolution_vars.push_back(var);
            if (is_lia)
            {
                lia_var_vec.push_back((int)_resolution_vars.size() - 1);
            }
            else
            {
                bool_var_vec.push_back((int)_resolution_vars.size() - 1);
            }
            return _resolution_vars.size() - 1;
        }
        else
            return name2resolution_var[name];
    }

    uint64_t ls_solver::transfer_name_to_tmp_var(std::string &name)
    {
        if (name2tmp_var.find(name) == name2tmp_var.end())
        {
            name2tmp_var[name] = _tmp_vars.size();
            variable var;
            var.is_lia = true;
            var.var_name = name;
            _tmp_vars.push_back(var);
            return _tmp_vars.size() - 1;
        }
        else
            return name2tmp_var[name];
    }
    
    uint64_t ls_solver::transfer_name_to_tmp_var1(std::string &name, int flag)
    {
        if (name2tmp_var.find(name) == name2tmp_var.end())
        {
            // if(flag==1) std::cout<<"nooooo!"<<name<<std::endl;
            name2tmp_var[name] = _tmp_vars.size();
            variable var;
            var.is_lia = true;
            var.var_name = name;
            _tmp_vars.push_back(var);
            return _tmp_vars.size() - 1;
        }
        else
        {
            // if(flag==1) std::cout<<name2tmp_var[name]<<std::endl;
            return name2tmp_var[name];
        }
    }
    // transfer the lia var into _resolution_vars, turn (x-y) to z
    void ls_solver::reduce_vars()
    {
        const uint64_t tmp_vars_size = _tmp_vars.size();
        std::vector<int> hash_map(tmp_vars_size * tmp_vars_size, 0); // hash_map[A*(size)+b]=n means A-B has occurred n times
        std::vector<int> occur_time(tmp_vars_size, 0);               // occur_time[a]=n means that a has occured in lits for n times
        pair_x = new Array((int)tmp_vars_size);
        pair_y = new Array((int)tmp_vars_size);
        lit *l;
        variable *original_var;
        variable *new_var;
        std::string var_name;
        int pos_var_idx, neg_var_idx, original_var_idx;
        use_pbs = !(_resolution_vars.size() == 0);
        for (int var_idx = 0; var_idx < tmp_vars_size; var_idx++)
        {
            if (_tmp_vars[var_idx].upper_bound > 1 || _tmp_vars[var_idx].low_bound < 0)
            {
                use_pbs = false;
                break;
            }
        }
        if (use_pbs)
        {
            _resolution_vars = _tmp_vars;
        } // if there is no boolean vars and all lia vars are in [0,1], then use pbs, and no need to reduce the vars
        else
        {
            // calculate the hash_map
            for (uint64_t l_idx = 0; l_idx < _num_lits; l_idx++)
            {
                l = &(_lits[l_idx]);
                if (l->lits_index == 0)
                {
                    continue;
                }
                for (int i = 0; i < l->pos_coff.size(); i++)
                {
                    pos_var_idx = l->pos_coff_var_idx[i];
                    for (int j = 0; j < l->neg_coff.size(); j++)
                    {
                        if (l->pos_coff[i] != l->neg_coff[j])
                        {
                            continue; // 只针对A-B的情况
                        }
                        neg_var_idx = l->neg_coff_var_idx[j];
                        if (neg_var_idx < pos_var_idx)
                        {
                            hash_map[neg_var_idx * tmp_vars_size + pos_var_idx]++;
                        } // small_idx* num_var+ large_idx
                        else
                        {
                            hash_map[pos_var_idx * tmp_vars_size + neg_var_idx]++;
                        }
                    }
                }
            }
            // calculate the occur time
            for (uint64_t l_idx = 0; l_idx < _num_lits; l_idx++)
            {
                l = &(_lits[l_idx]);
                if (l->lits_index == 0 || !l->is_lia_lit)
                {
                    continue;
                }
                for (int i = 0; i < l->pos_coff.size(); i++)
                {
                    occur_time[l->pos_coff_var_idx[i]]++;
                }
                for (int i = 0; i < l->neg_coff.size(); i++)
                {
                    occur_time[l->neg_coff_var_idx[i]]++;
                }
            }
            // calculate the x-y pair
            for (int pre_idx = 0; pre_idx < tmp_vars_size - 1; pre_idx++)
            {
                if (pair_y->is_in_array(pre_idx) || occur_time[pre_idx] == 0) //?
                {
                    continue;
                } // prevent reinsert
                for (int pos_idx = pre_idx + 1; pos_idx < tmp_vars_size; pos_idx++)
                {
                    if (pair_y->is_in_array(pos_idx))
                    {
                        continue;
                    } // prevent reinsert
                    if (hash_map[pre_idx * tmp_vars_size + pos_idx] == occur_time[pre_idx] && occur_time[pre_idx] == occur_time[pos_idx])
                    {
                        pair_x->insert_element(pre_idx);
                        pair_y->insert_element(pos_idx);
                        break; // dont completely know
                    }
                }
            }
            name2var.clear();
            // rewrite lits
            for (uint64_t l_idx = 0; l_idx < _num_lits; l_idx++)
            {
                l = &(_lits[l_idx]);
                lit new_lit;
                if (l->lits_index == 0 || !l->is_lia_lit)
                {
                    continue;
                }
                new_lit.key = l->key;
                new_lit.lits_index = l->lits_index;
                new_lit.is_equal = l->is_equal;
                new_lit.is_lia_lit = l->is_lia_lit;
                for (int i = 0; i < l->pos_coff.size(); i++)
                {
                    original_var_idx = l->pos_coff_var_idx[i];
                    original_var = &(_tmp_vars[original_var_idx]);
                    //     if (pair_x->is_in_array(original_var_idx))
                    //     {
                    //         new_lit.pos_coff.push_back(l->pos_coff[i]);
                    //         //                var_name="_new_var_"+std::to_string(pair_x->index_of(original_var_idx));
                    //         var_name = "_new_var_" + original_var->var_name;
                    //         new_lit.pos_coff_var_idx.push_back((int)transfer_name_to_resolution_var(var_name, true));
                    //     }
                    //     else if (pair_y->is_in_array(original_var_idx))
                    //     {
                    //         new_lit.neg_coff.push_back(l->pos_coff[i]);
                    //         //                var_name="_new_var_"+std::to_string(pair_y->index_of(original_var_idx));
                    //         var_name = "_new_var_" + _tmp_vars[pair_x->element_at(pair_y->index_of(original_var_idx))].var_name;
                    //         new_lit.neg_coff_var_idx.push_back((int)transfer_name_to_resolution_var(var_name, true));
                    //     }
                    //     else
                    //     {
                    new_lit.pos_coff.push_back(l->pos_coff[i]);
                    new_lit.pos_coff_var_idx.push_back((int)transfer_name_to_resolution_var(original_var->var_name, true));
                    //}
                }
                for (int i = 0; i < l->neg_coff.size(); i++)
                {
                    original_var_idx = l->neg_coff_var_idx[i];
                    original_var = &(_tmp_vars[original_var_idx]);
                    // if (!pair_x->is_in_array(original_var_idx) && !pair_y->is_in_array(original_var_idx))
                    // {
                    new_lit.neg_coff.push_back(l->neg_coff[i]);
                    new_lit.neg_coff_var_idx.push_back((int)transfer_name_to_resolution_var(original_var->var_name, true));
                    //}
                }
                _lits[l_idx] = new_lit;
            }
            // set low and upbound
            for (original_var_idx = 0; original_var_idx < _tmp_vars.size(); original_var_idx++)
            {
                original_var = &(_tmp_vars[original_var_idx]);
                if (occur_time[original_var_idx] == 0)
                {
                    continue;
                }
                // original var
                //  if (!pair_x->is_in_array(original_var_idx) && !pair_y->is_in_array(original_var_idx))
                //  {
                new_var = &(_resolution_vars[transfer_name_to_resolution_var(original_var->var_name, true)]);
                new_var->low_bound = original_var->low_bound;
                // std::cout<<new_var->low_bound;
                new_var->upper_bound = original_var->upper_bound;
                //}
                // new var
                // else if (pair_x->is_in_array(original_var_idx))
                // {
                //     int pair_idx = pair_x->index_of(original_var_idx);
                //     //            var_name="_new_var_"+std::to_string(pair_idx);
                //     var_name = "_new_var_" + original_var->var_name;
                //     new_var = &(_resolution_vars[transfer_name_to_resolution_var(var_name, true)]);
                //     __int128_t x_low = original_var->low_bound;
                //     __int128_t x_upper = original_var->upper_bound;
                //     __int128_t y_low = _tmp_vars[pair_y->element_at(pair_idx)].low_bound;
                //     __int128_t y_upper = _tmp_vars[pair_y->element_at(pair_idx)].upper_bound;
                //     if (x_low == -max_int || y_upper == max_int)
                //     {
                //         new_var->low_bound = -max_int;
                //     }
                //     else
                //     {
                //         new_var->low_bound = x_low - y_upper;
                //     }
                //     if (x_upper == max_int || y_low == -max_int)
                //     {
                //         new_var->upper_bound = max_int;
                //     }
                //     else
                //     {
                //         new_var->upper_bound = x_upper - y_low;
                //     }
                // }
            }
        }
        variable *cur;
        for (uint64_t lit_i = 0; lit_i < _bound_lits.size(); lit_i++)
        {
            l = &_lits[(_bound_lits[lit_i])];
            if (l->neg_coff.size() == 1 && l->pos_coff.size() == 0)
            {
                cur = &(_resolution_vars[l->neg_coff_var_idx[0]]);
                if (cur->low_bound != -max_int && cur->upper_bound != -max_int)
                {
                    if (_bound_clauses[lit_i] > 0)
                        l->lits_index = -2;
                    else
                        l->lits_index = -1;
                }
                else
                {
                    cur->low_bound = -max_int;
                    cur->upper_bound = -max_int;
                }
            }
            else if (l->neg_coff.size() == 0 && l->pos_coff.size() == 1)
            {
                cur = &(_resolution_vars[l->pos_coff_var_idx[0]]);
                if (cur->low_bound != -max_int && cur->upper_bound != -max_int)
                {
                    if (_bound_clauses[lit_i] > 0)
                        l->lits_index = -2;
                    else
                        l->lits_index = -1;
                }
                else
                {
                    cur->low_bound = -max_int;
                    cur->upper_bound = -max_int;
                }
            }
        }

        // int num_var_with_bound = 0;
        // for (int var_idx = 0; var_idx < _resolution_vars.size(); var_idx++)
        // {
        //     new_var = &(_resolution_vars[var_idx]);
        //     if (!new_var->is_lia)
        //     {
        //         continue;
        //     }
        //     if (new_var->upper_bound != max_int && new_var->low_bound != -max_int)
        //     {
        //         continue;
        //     } //if a var has both upper bound and lower bound, no bound lits is added.
        //     if (new_var->low_bound != -max_int)
        //     {
        //         int lit_idx = _bound_lits[num_var_with_bound++];//unit clause : lit index=0, (5) (6[lit index=0]) (6 7)
        //         lit bound_lit;
        //         bound_lit.is_lia_lit = true;
        //         bound_lit.lits_index = lit_idx;
        //         bound_lit.neg_coff.push_back(1);
        //         bound_lit.neg_coff_var_idx.push_back(var_idx);//zheli youwenti
        //         bound_lit.key = new_var->low_bound;//6 lit index=0
        //         _lits[lit_idx] = bound_lit;//5a 6b (7 6 )//lits[6].lit_index=6
        //         //new_var->low_bound = -max_int;
        //     }
        //     if (new_var->upper_bound != max_int)
        //     {
        //         int lit_idx = _bound_lits[num_var_with_bound++];
        //         lit bound_lit;
        //         bound_lit.is_lia_lit = true;
        //         bound_lit.lits_index = lit_idx;
        //         bound_lit.pos_coff.push_back(1);
        //         bound_lit.pos_coff_var_idx.push_back(var_idx);
        //         bound_lit.key = -new_var->upper_bound;
        //         _lits[lit_idx] = bound_lit;
        //         //new_var->upper_bound = max_int;
        //     }
        // }
    }

    void ls_solver::unit_prop()
    {
        std::stack<uint64_t> unit_clause; // the var_idx in the unit clause
        for (uint64_t clause_idx = 0; clause_idx < _num_clauses; clause_idx++)
        { // the unit clause is the undeleted clause containing only one bool var
            if (!_clauses[clause_idx].is_delete && _clauses[clause_idx].literals.size() == 1 && !_lits[std::abs(_clauses[clause_idx].literals[0])].is_lia_lit)
            {
                unit_clause.push(clause_idx);
            }
        }
        while (!unit_clause.empty())
        {
            uint64_t unit_clause_idx = unit_clause.top();
            unit_clause.pop();
            if (_clauses[unit_clause_idx].is_delete)
            {
                continue;
            }
            int is_pos_lit = (_clauses[unit_clause_idx].literals[0] > 0) ? 1 : -1;            // determine the sign of the var in unit clause
            uint64_t unit_var = _lits[std::abs(_clauses[unit_clause_idx].literals[0])].delta; // determing the var in unit clause
            _resolution_vars[unit_var].is_delete = true;                                      // delete the unit var
            for (uint64_t cl_idx : _resolution_vars[unit_var].clause_idxs)
            {
                clause *cl = &(_clauses[cl_idx]);
                if (cl->is_delete)
                    continue;
                for (uint64_t lit_idx = 0; lit_idx < cl->literals.size(); lit_idx++)
                {
                    int l_id_in_lits = cl->literals[lit_idx];
                    lit *l = &(_lits[std::abs(l_id_in_lits)]);
                    if (!l->is_lia_lit && l->delta == unit_var)
                    { // go through the clauses of the unit var, find the var in corresponding clause
                        if ((is_pos_lit > 0 && l_id_in_lits > 0) || (is_pos_lit < 0 && l_id_in_lits < 0))
                        {
                            cl->is_delete = true;
                            for (int l_idx_inner : cl->literals)
                            { // delete the clause from corresponding bool var
                                lit *l_inner = &(_lits[std::abs(l_idx_inner)]);
                                if (!l_inner->is_lia_lit && l_inner->delta != unit_var)
                                {
                                    variable *var_inner = &(_resolution_vars[l_inner->delta]);
                                    for (uint64_t delete_idx = 0; delete_idx < var_inner->clause_idxs.size(); delete_idx++)
                                    {
                                        if (var_inner->clause_idxs[delete_idx] == cl_idx)
                                        {
                                            var_inner->clause_idxs[delete_idx] = var_inner->clause_idxs.back();
                                            var_inner->clause_idxs.pop_back();
                                            break;
                                        }
                                    }
                                }
                            }
                        } // if there exist same lit, the clause is already set true, then delete the clause
                        else
                        { // else delete the lit
                            cl->literals[lit_idx] = cl->literals.back();
                            cl->literals.pop_back();
                            if (cl->literals.size() == 1 && !_lits[std::abs(cl->literals[0])].is_lia_lit)
                            {
                                unit_clause.push(cl_idx);
                            } // if after deleting, it becomes unit clause
                        }
                        break;
                    }
                }
            }
        }
    }

    void ls_solver::resolution()
    {
        std::vector<uint64_t> pos_clauses(10 * _num_clauses);
        std::vector<uint64_t> neg_clauses(10 * _num_clauses);
        int pos_clause_size, neg_clause_size;
        bool is_improve = true;
        // int numc=0;
        while (is_improve) //?
        {
            is_improve = false;
            for (uint64_t bool_var_idx : bool_var_vec)
            {
                if (_resolution_vars[bool_var_idx].is_delete)
                    continue;
                pos_clause_size = 0;
                neg_clause_size = 0;
                for (int i = 0; i < _resolution_vars[bool_var_idx].clause_idxs.size(); i++)
                {
                    uint64_t clause_idx = _resolution_vars[bool_var_idx].clause_idxs[i];
                    if (_clauses[clause_idx].is_delete)
                        continue;
                    for (int l_var_sign : _clauses[clause_idx].literals)
                    {
                        if (!_lits[std::abs(l_var_sign)].is_lia_lit && _lits[std::abs(l_var_sign)].delta == bool_var_idx)
                        { // make sure that it is a boolean literal and is exactly the one containing the var
                            if (l_var_sign > 0)
                            {
                                pos_clauses[pos_clause_size++] = clause_idx;
                            }
                            else
                            {
                                neg_clauses[neg_clause_size++] = clause_idx;
                            }
                            break;
                        }
                    }
                } // determine the pos_clause and neg_clause
                int tautology_num = 0;
                for (int i = 0; i < pos_clause_size; i++)
                { // pos clause X neg clause
                    uint64_t pos_clause_idx = pos_clauses[i];
                    for (int j = 0; j < neg_clause_size; j++)
                    {
                        uint64_t neg_clause_idx = neg_clauses[j];
                        for (int k = 0; k < _clauses[neg_clause_idx].literals.size(); k++)
                        {
                            int l_neg_lit = _clauses[neg_clause_idx].literals[k];
                            if (_lits[std::abs(l_neg_lit)].delta != bool_var_idx || _lits[std::abs(l_neg_lit)].is_lia_lit)
                            { // the bool_var for resolution is not considered(that is \neg ( the lit is bool lit and it contains the var)) why?
                                for (int l_pos_lit : _clauses[pos_clause_idx].literals)
                                {
                                    if (-l_neg_lit == (l_pos_lit))
                                    {
                                        tautology_num++;
                                        break;
                                    } // if there exists (aVb)^(-aV-b), the new clause is tautology
                                }
                            }
                        }
                    }
                }
                if ((pos_clause_size * neg_clause_size - tautology_num) > (pos_clause_size + neg_clause_size))
                {
                    continue;
                } // if deleting the var can cause 2 times clauses, then skip it
                for (uint64_t clause_idx : _resolution_vars[bool_var_idx].clause_idxs)
                { // delete the clauses of bool_var
                    _clauses[clause_idx].is_delete = true;
                    for (int l_idx_sign : _clauses[clause_idx].literals)
                    { // delete the clause from corresponding bool var
                        lit *l = &(_lits[std::abs(l_idx_sign)]);
                        if (!l->is_lia_lit && l->delta != bool_var_idx)
                        {
                            variable *var_inner = &(_resolution_vars[l->delta]);
                            for (uint64_t delete_idx = 0; delete_idx < var_inner->clause_idxs.size(); delete_idx++)
                            {
                                if (var_inner->clause_idxs[delete_idx] == clause_idx)
                                {
                                    var_inner->clause_idxs[delete_idx] = var_inner->clause_idxs.back();
                                    var_inner->clause_idxs.pop_back();
                                    break;
                                }
                            }
                        }
                    }
                }
                is_improve = true;
                _resolution_vars[bool_var_idx].is_delete = true;
                if (pos_clause_size == 0 || neg_clause_size == 0)
                    continue; // pos or neg clause is empty, meaning the clauses are SAT when assigned to true or false, then cannot resolute, just delete the clause
                for (int i = 0; i < pos_clause_size; i++)
                { // pos clause X neg clause
                    uint64_t pos_clause_idx = pos_clauses[i];
                    for (int j = 0; j < neg_clause_size; j++)
                    {
                        uint64_t neg_clause_idx = neg_clauses[j];
                        clause new_clause;
                        uint64_t pos_lit_size = _clauses[pos_clause_idx].literals.size();
                        uint64_t neg_lit_size = _clauses[neg_clause_idx].literals.size();
                        new_clause.literals.reserve(pos_lit_size + neg_lit_size);
                        bool is_tautology = false;
                        for (int k = 0; k < pos_lit_size; k++)
                        {
                            int l_sign_idx = _clauses[pos_clause_idx].literals[k];
                            if (_lits[std::abs(l_sign_idx)].is_lia_lit || _lits[std::abs(l_sign_idx)].delta != bool_var_idx)
                            {
                                new_clause.literals.push_back(l_sign_idx);
                            }
                        } // add the lits in pos clause to new clause
                        for (int k = 0; k < neg_lit_size; k++)
                        {
                            int l_sign_idx = _clauses[neg_clause_idx].literals[k];
                            if (_lits[std::abs(l_sign_idx)].is_lia_lit || _lits[std::abs(l_sign_idx)].delta != bool_var_idx)
                            {
                                bool is_existed_lit = false;
                                for (uint64_t i = 0; i < pos_lit_size - 1; i++) // 新子句里只有pos-1个literal
                                {
                                    if (l_sign_idx == (new_clause.literals[i]))
                                    {
                                        is_existed_lit = true;
                                        break;
                                    } // if the lit has existed, then it will not be added
                                    if (l_sign_idx == (-new_clause.literals[i]))
                                    {
                                        is_tautology = true;
                                        break;
                                    } // if there exists (aVb)^(-aV-b), the new clause is tautology
                                }
                                if (is_tautology)
                                {
                                    break;
                                }
                                if (!is_existed_lit)
                                {
                                    new_clause.literals.push_back(l_sign_idx);
                                }
                            }
                        }
                        if (!is_tautology)
                        { // add new clause, and modify the clause of corresponding bool var
                            for (int l_sign_idx : new_clause.literals)
                            {
                                lit *l_inner = &(_lits[std::abs(l_sign_idx)]);
                                if (!l_inner->is_lia_lit)
                                {
                                    _resolution_vars[l_inner->delta].clause_idxs.push_back((int)_num_clauses);
                                }
                            }
                            _clauses.push_back(new_clause);
                            _num_clauses++;
                        }
                    }
                }
            }
        }
        // std::cout<<"num:"<<numc<<std::endl;
    }

    void ls_solver::reduce_clause()
    {
        bool_var_vec.clear();
        lia_var_vec.clear();
        _vars.reserve(_resolution_vars.size());
        int i, reduced_clause_num;
        i = 0;
        reduced_clause_num = 0;
        for (; i < _num_clauses; i++)
        {
            if (!_clauses[i].is_delete)
            {
                _clauses[reduced_clause_num++] = _clauses[i];
            }
        }
        _clauses.resize(reduced_clause_num);
        _num_lia_lits = 0;
        _num_bool_lits = 0;
        for (int l_idx = 0; l_idx < _lits.size(); l_idx++)
        {
            lit *l = &(_lits[l_idx]);
            // std::cout<<l->lits_index<<std::endl;
            if (l->lits_index == 0)
            {
                continue;
            }
            if (l->is_lia_lit)
            {
                _num_lia_lits++;
            }
            else
            {
                if (!_resolution_vars[l->delta].is_delete)
                {
                    _num_bool_lits++;
                }
                else
                {
                    l->lits_index = 0;
                }
            }
        } // transfer the resolution vars to vars

        _num_clauses = reduced_clause_num;
        // unit prop
        std::vector<int> unit_lit;
        std::vector<int> unit_clause;
        for (int clause_idx = 0; clause_idx < _num_clauses; clause_idx++)
        {
            if (_clauses[clause_idx].literals.size() == 1)
            {
                unit_clause.push_back(clause_idx);
                unit_lit.push_back(_clauses[clause_idx].literals[0]);
            }
        }
        // for (int clause_idx = 0; clause_idx < _num_clauses; clause_idx++)
        // {
        //     if(find(unit_clause.begin(),unit_clause.end(),clause_idx)!=unit_clause.end()) continue;
        //     std::vector<int>used_lit;
        //     for (int k = 0; k < _clauses[clause_idx].literals.size(); k++)
        //     {
        //         int l_sign_idx = _clauses[clause_idx].literals[k];
        //         if(find(unit_lit.begin(),unit_lit.end(),l_sign_idx)!=unit_lit.end())
        //         {
        //             _clauses[clause_idx].is_delete=true;
        //             break;
        //         }
        //         if(find(unit_lit.begin(),unit_lit.end(),-l_sign_idx)==unit_lit.end())
        //         {
        //             used_lit.push_back(l_sign_idx);
        //         }
        //     }
        //     _clauses[clause_idx].literals=used_lit;
        // }
        // reduced_clause_num = 0;
        // for (int i=0; i < _num_clauses; i++)
        // {
        //     if (!_clauses[i].is_delete)
        //     {
        //         _clauses[reduced_clause_num++] = _clauses[i];
        //     }
        // }
        // _num_clauses = reduced_clause_num;
        // unit prop
        std::vector<bool> lit_appear(_num_lits + _additional_len, false);
        for (int clause_idx = 0; clause_idx < reduced_clause_num; clause_idx++)
        {
            _clauses[clause_idx].weight = 1;
            for (int k = 0; k < _clauses[clause_idx].literals.size(); k++)
            {
                int l_sign_idx = _clauses[clause_idx].literals[k];
                lit *l = &(_lits[std::abs(l_sign_idx)]);
                if (l->is_lia_lit)
                {
                    variable *v;
                    for (int j = 0; j < l->neg_coff.size(); j++)
                    {
                        if (!lit_appear[l->lits_index])
                        {
                            l->neg_coff_var_idx[j] = (int)transfer_name_to_reduced_var(_resolution_vars[l->neg_coff_var_idx[j]].var_name, true);
                        }
                        v = &(_vars[l->neg_coff_var_idx[j]]);
                        v->literals.push_back(l_sign_idx);
                        v->literal_clause.push_back(clause_idx);
                        v->literal_coff.push_back(-l->neg_coff[j]);
                    }
                    for (int j = 0; j < l->pos_coff.size(); j++)
                    {
                        if (!lit_appear[l->lits_index])
                        {
                            l->pos_coff_var_idx[j] = (int)transfer_name_to_reduced_var(_resolution_vars[l->pos_coff_var_idx[j]].var_name, true);
                        }
                        v = &(_vars[l->pos_coff_var_idx[j]]);
                        v->literals.push_back(l_sign_idx);
                        v->literal_clause.push_back(clause_idx);
                        v->literal_coff.push_back(l->pos_coff[j]);
                    }
                    _clauses[clause_idx].lia_literals.push_back(l_sign_idx);
                }
                else
                {
                    if (!lit_appear[l->lits_index])
                    {
                        l->delta = transfer_name_to_reduced_var(_resolution_vars[l->delta].var_name, false);
                    }
                    _vars[l->delta].literals.push_back(l_sign_idx);
                    _vars[l->delta].literal_clause.push_back(clause_idx);
                    _clauses[clause_idx].bool_literals.push_back(l_sign_idx);
                }
                if (!lit_appear[l->lits_index])
                {
                    lit_appear[l->lits_index] = true;
                }
            }
        }

        // soft
        std::vector<int> soft;
        std::vector<int> soft_w;

        reduced_bound = 0;
        for (int i = 0; i < soft_clause_lit.size(); i++)
        {
            if (find(unit_lit.begin(), unit_lit.end(), soft_clause_lit[i]) != unit_lit.end())
            {
                continue;
            }
            if (find(unit_lit.begin(), unit_lit.end(), -soft_clause_lit[i]) != unit_lit.end())
            {
                reduced_bound += soft_clause_weight[i];
                continue;
            }
            soft.push_back(soft_clause_lit[i]);
            soft_w.push_back(soft_clause_weight[i]);
        }
        // int k=2;
        // float soft_ave_weight;
        // for(int i=0;i<soft_w.size();i++)
        // {
        //     soft_ave_weight+=soft_w[i];
        // }
        // soft_ave_weight=soft_ave_weight/soft_w.size();
        // for(int i=0;i<soft_w.size();i++)
        // {
        //     float rate=soft_w[i]/soft_ave_weight;
        //     soft_inc.push_back(rate*k+1);
        // }
        for (int i = 0; i < soft_w.size(); i++)
        {
            float weight_num = 150;
            int inc = soft_w[i] / weight_num;
            soft_inc.push_back(inc + 1);
        }
        // std::vector<int>soft_weight_st=soft_w;
        // sort(soft_weight_st.begin(),soft_weight_st.end());
        // for(int i=0;i<soft_w.size();i++)
        // {
        //     int idx=find(soft_weight_st.begin(),soft_weight_st.end(),soft_w[i])-soft_weight_st.begin()+10;
        //     if(idx<soft_weight_st.size())
        //     {
        //         if(soft_weight_st[idx]!=soft_w[i]) soft_threshold.push_back(soft_weight_st[idx]-1);
        //         else soft_threshold.push_back(soft_weight_st[idx]+1);
        //     }
        //     else
        //     {
        //         soft_threshold.push_back(soft_w[i]*1.5);
        //     }
        // }
        // for(int i=0;i<soft_threshold.size();i++)
        // std::cout<<soft_w[i]<<"   "<<soft_threshold[i]<<std::endl;
        soft_clause_lit = soft;
        soft_clause_weight = soft_w;
        for (int i = 0; i < soft_clause_lit.size(); i++)
        {
            // if(find(unit_lit.begin(),unit_lit.end(),soft_clause_lit[i])!=unit_lit.end())
            // {
            //     continue;
            // }
            // if(find(unit_lit.begin(),unit_lit.end(),-soft_clause_lit[i])!=unit_lit.end())
            // {
            //     init_bound+=soft_clause_weight[i];
            //     continue;
            // }
            soft_clause cur;
            cur.literals.push_back(soft_clause_lit[i]);
            // std::cout<<soft_clause_lit[i]<<std::endl;
            int l_sign_idx = soft_clause_lit[i];
            lit *l = &(_lits[std::abs(l_sign_idx)]);
            cur.weight = soft_clause_weight[i]; // to be revise
            cur.org_weight = cur.weight;

            if (l->is_lia_lit)
            {
                cur.lia_literals.push_back(l_sign_idx);
            }
            else
            {
                cur.bool_literals.push_back(l_sign_idx);
            }
            _soft_clauses.push_back(cur);
            if (l->is_lia_lit)
            {
                variable *v;
                for (int j = 0; j < l->neg_coff.size(); j++)
                {
                    if (!lit_appear[l->lits_index])
                    {
                        l->neg_coff_var_idx[j] = (int)transfer_name_to_reduced_var(_resolution_vars[l->neg_coff_var_idx[j]].var_name, true);
                    }
                    v = &(_vars[l->neg_coff_var_idx[j]]);
                    v->soft_literals.push_back(l_sign_idx);
                    v->soft_literal_clause.push_back(i);
                    v->soft_literal_coff.push_back(-l->neg_coff[j]);
                }
                for (int j = 0; j < l->pos_coff.size(); j++)
                {
                    if (!lit_appear[l->lits_index])
                    {
                        l->pos_coff_var_idx[j] = (int)transfer_name_to_reduced_var(_resolution_vars[l->pos_coff_var_idx[j]].var_name, true);
                    }
                    v = &(_vars[l->pos_coff_var_idx[j]]);
                    v->soft_literals.push_back(l_sign_idx);
                    v->soft_literal_clause.push_back(i);
                    v->soft_literal_coff.push_back(l->pos_coff[j]);
                }
            }
            else
            {
                // bool to be added
            }
            if (!lit_appear[l->lits_index])
            {
                lit_appear[l->lits_index] = true;
            }
        }
        _num_soft_clauses = _soft_clauses.size();
        // for(int i=0;i<_soft_clauses.size();i++)
        // {
        //     for(int j=0;j<_soft_clauses[i].literals.size();j++)
        //     {
        //         std::cout<<_soft_clauses[i].literals[j]<<std::endl;
        //     }
        // }
        // for (variable &v : _vars)
        // {
        //     for(int j=0;j<v.soft_literal_clause.size();j++)
        //     {
        //         std::cout<<v.soft_literal_clause[j]<<" ";
        //     }
        //     std::cout<<std::endl;
        // }
        // soft

        // determine the literals of vars
        _vars.resize(_vars.size());
        _num_vars = _vars.size();
        _num_lia_vars = 0;
        for (variable &v : _vars)
        {
            int pre_clause_idx = INT32_MAX;
            for (int i = 0; i < v.literal_clause.size(); i++)
            {
                int tmp_clause_idx = v.literal_clause[i];
                if (pre_clause_idx != tmp_clause_idx) //?
                {
                    v.clause_idxs.push_back(tmp_clause_idx);
                    pre_clause_idx = tmp_clause_idx;
                }
            }
            // soft
            int pre_soft_clause_idx = INT32_MAX;
            for (int j = 0; j < v.soft_literal_clause.size(); j++)
            {
                int tmp_soft_clause_idx = v.soft_literal_clause[j];
                if (pre_soft_clause_idx != tmp_soft_clause_idx) //?
                {
                    v.soft_clause_idxs.push_back(tmp_soft_clause_idx);
                    pre_soft_clause_idx = tmp_soft_clause_idx;
                }
            }
            // soft
            if (v.is_lia)
            {
                v.upper_bound = _resolution_vars[transfer_name_to_resolution_var(v.var_name, true)].upper_bound;
                v.low_bound = _resolution_vars[transfer_name_to_resolution_var(v.var_name, true)].low_bound;
                _num_lia_vars++;
            }
        } // determine the clause_idxs of var
        // for (variable &v : _vars)
        // {
        //     for (uint64_t i = 0; i < v.soft_clause_idxs.size(); i++)
        //     {
        //         std::cout<<v.soft_clause_idxs[i]<<" ";
        //     }
        //     std::cout<<std::endl;
        // }
        __int128_t max_lit_num = 0;
        for (int var_idx = 0; var_idx < _num_vars; var_idx++)
        {
            if (!_vars[var_idx].is_lia)
            {
                continue;
            }
            if (_vars[var_idx].literals.size() > max_lit_num)
            {
                lia_var_idx_with_most_lits = var_idx;
                max_lit_num = _vars[var_idx].literals.size();
            }
        } // dont know meaning,to be revise
        for (int lit_idx = 0; lit_idx < _lits.size(); lit_idx++)
        {
            lit *l = &(_lits[lit_idx]);
            if (l->lits_index == 0 || !l->is_lia_lit)
            {
                continue;
            }
            if (l->pos_coff.size() != 1 || l->neg_coff.size() != 1 || l->pos_coff[0] != 1 || l->neg_coff[0] != 1)
            {
                lia_var_idx_with_most_lits = -1;
                is_idl = false;
                break;
            } // var_with most lit are dedicated for IDL
        }
    }

    void ls_solver::set_pre_value()
    {
        _pre_value_1.resize(_num_vars + _additional_len, INT32_MAX);
        _pre_value_2.resize(_num_vars + _additional_len, INT32_MAX);
        for (clause &cl : _clauses)
        {
            if (cl.literals.size() == 1 && cl.literals[0] > 0 && _lits[cl.literals[0]].is_equal)
            {
                lit *l = &(_lits[cl.literals[0]]);
                if (l->pos_coff.size() + l->neg_coff.size() == 1)
                {
                    if (l->pos_coff.size() == 1)
                    {
                        _pre_value_1[l->pos_coff_var_idx[0]] = (int)(-l->key / l->pos_coff[0]);
                    }
                    else if (l->neg_coff.size() == 1)
                    {
                        _pre_value_1[l->neg_coff_var_idx[0]] = (int)(l->key / l->neg_coff[0]);
                    }
                }
            } //(a==0)
            else if (cl.literals.size() == 2 && cl.literals[0] > 0 && _lits[cl.literals[0]].is_equal && cl.literals[1] > 0 && _lits[cl.literals[1]].is_equal)
            {
                lit *l1 = &(_lits[cl.literals[0]]);
                lit *l2 = &(_lits[cl.literals[1]]);
                if ((l1->pos_coff.size() + l1->neg_coff.size() == 1) && (l2->pos_coff.size() + l2->neg_coff.size() == 1))
                {
                    int var_idx_1 = (l1->pos_coff.size() == 1) ? (l1->pos_coff_var_idx[0]) : (l1->neg_coff_var_idx[0]);
                    int var_idx_2 = (l2->pos_coff.size() == 1) ? (l2->pos_coff_var_idx[0]) : (l2->neg_coff_var_idx[0]);
                    if (var_idx_1 == var_idx_2)
                    {
                        _pre_value_1[var_idx_1] = (l1->pos_coff.size() == 1) ? ((int)(-l1->key / l1->pos_coff[0])) : ((int)(l1->key / l1->neg_coff[0]));
                        _pre_value_2[var_idx_1] = (l2->pos_coff.size() == 1) ? ((int)(-l2->key / l2->pos_coff[0])) : ((int)(l2->key / l2->neg_coff[0]));
                    }
                }
            } //(a==0 OR a==1)
        }
    }
    // initialize
    ls_solver::ls_solver()
        : _swt_p(0.3),
          _swt_threshold(50),
          smooth_probability(100), // 5
          _cutoff(300),
          _additional_len(10),
          _max_step(UINT64_MAX),
          CC_mode(-1)
    {
        mt.seed(1);
    }

    ls_solver::ls_solver(int random_seed)
        : _swt_p(0.3),
          _swt_threshold(50),
          smooth_probability(100), // 5
          _cutoff(300),
          _additional_len(10),
          _max_step(UINT64_MAX),
          CC_mode(-1)
    {
        mt.seed(random_seed);
    }

    void ls_solver::make_space()
    {
        _solution.resize(_num_vars + _additional_len);
        _best_solutin.resize(_num_vars + _additional_len);
        tabulist.resize(2 * _num_vars + _additional_len, 0);
        CClist.resize(2 * _num_vars + _additional_len, 1);
        operation_var_idx_vec.resize(_num_opt + _additional_len);
        operation_var_idx_bool_vec.resize(_num_opt + _additional_len);
        operation_change_value_vec.resize(_num_opt + _additional_len);
        last_move.resize(2 * _num_vars + _additional_len, 0);
        unsat_clauses = new Array((int)_num_clauses + (int)_additional_len);
        unsat_soft_clauses = new Array((int)_num_soft_clauses + (int)_additional_len); // soft new
        sat_clause_with_false_literal = new Array((int)_num_clauses + (int)_additional_len);
        sat_soft_clause_with_false_literal = new Array((int)_num_soft_clauses + (int)_additional_len);
        lit_occur = new Array((int)_num_lits);
        contain_bool_unsat_clauses = new Array((int)_num_clauses);
        contain_bool_unsat_soft_clauses = new Array((int)_num_soft_clauses); // soft new
        is_chosen_bool_var.resize(_num_vars + _additional_len, false);
        lit_idx.resize(_num_opt + _additional_len);
    }

    void ls_solver::initialize()
    {
        clear_prev_data();
        construct_slution_score();
        initialize_lit_datas();
        initialize_clause_datas();
        initialize_variable_datas();
        best_found_this_restart = unsat_clauses->size();
        // for(int i=0;i<unsat_soft_clauses->size();i++)
        // {
        //     best_found_obj+=_soft_clauses[unsat_soft_clauses->element_at(i)].weight;
        // }
        if (issat == 0) //
        {
            best_found_obj = 0;
            for (int i = 0; i < _soft_clauses.size(); i++)
            {
                best_found_obj += _soft_clauses[i].org_weight;
            }
        }

        // std::cout<<"best fo"<<best_found_obj<<std::endl;
        // std::cout<<best_found_obj<<std::endl;
        // std::cout<<best_found_obj;
        update_best_solution();
    }

    void ls_solver::initialize_variable_datas()
    {
    }
    // initialize the delta of each literal by delta_lit operation
    void ls_solver::initialize_lit_datas()
    {
        for (int i = 0; i < _num_lits; i++)
        {
            if (_lits[i].lits_index != 0 && _lits[i].is_lia_lit)
            {
                _lits[i].delta = delta_lit(_lits[i]);
                // std::cout<<print_128(_lits[i].delta)<<std::endl;
            }
        }
    }
    // set the sat num of each clause, and sat/unsat a clause
    void ls_solver::initialize_clause_datas()
    {
        _lit_in_unsat_clause_num = 0;
        _bool_lit_in_unsat_clause_num = 0;
        _lit_in_unsat_soft_clause_num = 0;      // soft new
        _bool_lit_in_unsat_soft_clause_num = 0; // soft new
        __int128_t pos_delta;
        // if(issat==0){
        for (uint64_t c = 0; c < _num_clauses; c++)
        {
            clause *cl = &(_clauses[c]);
            cl->sat_count = 0;
            if (issat == 0 || flag_restart == 1)
                cl->weight = 1;
            cl->min_delta = INT64_MAX;
            for (int l_idx : cl->literals)
            {
                __int128_t delta = _lits[std::abs(l_idx)].delta;
                bool is_equal = _lits[std::abs(l_idx)].is_equal;
                if (!_lits[std::abs(l_idx)].is_lia_lit)
                {
                    if ((_solution[delta] > 0 && l_idx > 0) || (_solution[delta] < 0 && l_idx < 0))
                    {
                        cl->sat_count++;
                    }
                } // bool lit
                else if ((!is_equal && ((delta <= 0 && l_idx > 0) || (delta > 0 && l_idx < 0))) || (is_equal && ((delta == 0 && l_idx > 0) || (delta != 0 && l_idx < 0))))
                {
                    cl->sat_count++;
                } // lia lit
                pos_delta = delta;
                convert_to_pos_delta(pos_delta, l_idx);
                if (pos_delta < cl->min_delta)
                {
                    cl->min_delta = pos_delta;
                    cl->min_delta_lit_index = l_idx;
                }
            }
            if (cl->sat_count == 0)
            {
                unsat_a_clause(c);
                _lit_in_unsat_clause_num += _clauses[c].literals.size();
                _bool_lit_in_unsat_clause_num += _clauses[c].bool_literals.size();
                for (int l_sign_idx : cl->bool_literals)
                {
                    _vars[_lits[std::abs(l_sign_idx)].delta].score++;
                }
            }
            else
            {
                sat_a_clause(c);
            }
            if (cl->sat_count > 0 && cl->sat_count < cl->literals.size())
            {
                sat_clause_with_false_literal->insert_element((int)c);
            }
            // TODO::else{sat_clause_with_false_literal->delete_element((int)c);}
            if (cl->sat_count == 1)
            {
                lit *l = &(_lits[std::abs(cl->min_delta_lit_index)]);
                if (!l->is_lia_lit)
                {
                    _vars[l->delta].score--;
                }
            }
        }
        //}
        total_clause_weight = _num_clauses;

        // soft
        for (uint64_t c = 0; c < _soft_clauses.size(); c++)
        {
            soft_clause *cl = &(_soft_clauses[c]);
            cl->sat_count = 0;
            if (issat == 0)
                cl->weight = 1;
            else
                cl->weight = soft_clause_weight[c];
            cl->min_delta = INT64_MAX;
            for (int l_idx : cl->literals)
            {
                __int128_t delta = _lits[std::abs(l_idx)].delta;
                // std::cout<<print_128(delta)<<std::endl;
                bool is_equal = _lits[std::abs(l_idx)].is_equal;
                if (!_lits[std::abs(l_idx)].is_lia_lit)
                {
                    if ((_solution[delta] > 0 && l_idx > 0) || (_solution[delta] < 0 && l_idx < 0))
                    {
                        cl->sat_count++;
                    }
                } // bool lit to be revise
                else if ((!is_equal && ((delta <= 0 && l_idx > 0) || (delta > 0 && l_idx < 0))) || (is_equal && ((delta == 0 && l_idx > 0) || (delta != 0 && l_idx < 0))))
                {
                    cl->sat_count++;
                } // lia lit
                pos_delta = delta;
                convert_to_pos_delta(pos_delta, l_idx);
                if (pos_delta < cl->min_delta)
                {
                    cl->min_delta = pos_delta; // to be tested
                    cl->min_delta_lit_index = l_idx;
                }
            }
            // std::cout<<cl->min_delta_lit_index<<std::endl;
            if (cl->sat_count == 0)
            {
                unsat_a_soft_clause(c);
                _lit_in_unsat_soft_clause_num += _soft_clauses[c].literals.size();
                _bool_lit_in_unsat_soft_clause_num += _soft_clauses[c].bool_literals.size();
                for (int l_sign_idx : cl->bool_literals)
                {
                    std::cout << "bool to be added";
                    _vars[_lits[std::abs(l_sign_idx)].delta].score += cl->org_weight; // to be revise
                }
            }
            else
            {
                sat_a_soft_clause(c);
            }
            if (cl->sat_count > 0 && cl->sat_count < cl->literals.size())
            {
                sat_soft_clause_with_false_literal->insert_element((int)c);
            }
            // TODO::else{sat_clause_with_false_literal->delete_element((int)c);}
            if (cl->sat_count == 1)
            {
                lit *l = &(_lits[std::abs(cl->min_delta_lit_index)]);
                if (!l->is_lia_lit)
                {
                    std::cout << "bool to be added";
                    _vars[l->delta].score -= cl->org_weight; // to be revise
                }
            }
        }
        // for (int i = 0; i < unsat_soft_clauses->size(); i++)
        // {
        //     int unsat_cl = unsat_soft_clauses->element_at(i);
        //     std::cout<<unsat_cl<<std::endl;

        // }
        // std::cout<<sat_soft_clause_with_false_literal->size()<<std::endl;
        // soft
    }

    void ls_solver::build_neighbor()
    {
    }

    // random walk
    void ls_solver::update_clause_weight()
    {
        for (int i = 0; i < unsat_clauses->size(); i++)
        {
            clause *unsat_cl = &(_clauses[unsat_clauses->element_at(i)]);
            // unsat_cl->weight++;
            // if(issat==1&&unsat_cl->weight>1000) continue;
            // if(issat==1&&unsat_cl->weight>270) {continue;}//300 is best for no restart
            // if(_step>13000&&unsat_cl->weight>400) continue;
            // if(issat==1) h_inc=1;
            // if(unsat_cl->weight>450) continue;
            unsat_cl->weight += h_inc;
            for (int l_sign_idx : unsat_cl->bool_literals)
            {
                //_vars[_lits[std::abs(l_sign_idx)].delta].score++;
                _vars[_lits[std::abs(l_sign_idx)].delta].score += h_inc;
            }
        }
        // soft to be revise to fit weighted
        //  int cur_obj=0;
        //  for (int i = 0; i < unsat_soft_clauses->size(); i++)
        //  {
        //      cur_obj+=_soft_clauses[unsat_soft_clauses->element_at(i)].weight;
        //  }
        // if(is_weighted!=1&&issat==1)
        if (is_weighted == 1 && issat == 1)
        // if(is_weighted==1)
        {
            for (int i = 0; i < unsat_soft_clauses->size(); i++)
            {
                soft_clause *unsat_scl = &(_soft_clauses[unsat_soft_clauses->element_at(i)]);
                if (unsat_scl->weight < unsat_scl->org_weight * 1.5) // 这里有搞头
                // if(unsat_scl->weight<soft_threshold[unsat_soft_clauses->element_at(i)])
                // if(unsat_scl->weight<1300)
                // if(unsat_scl->weight<unsat_scl->org_weight&&issat==1)
                {
                    unsat_scl->weight += 1;
                    // unsat_scl->weight+=soft_inc[unsat_soft_clauses->element_at(i)];
                    for (int l_sign_idx : unsat_scl->bool_literals)
                    {
                        // _vars[_lits[std::abs(l_sign_idx)].delta].score+=soft_inc[unsat_soft_clauses->element_at(i)];
                        _vars[_lits[std::abs(l_sign_idx)].delta].score += 1;
                    }
                }
            }
        }
        else if (is_weighted != 1 && issat == 1)
        {
            for (int i = 0; i < unsat_soft_clauses->size(); i++)
            {
                soft_clause *unsat_scl = &(_soft_clauses[unsat_soft_clauses->element_at(i)]);
                // if(is_weighted==1&&issat==1&&unsat_scl->weight<unsat_scl->org_weight*1.5)
                if (unsat_scl->weight < 600)
                {
                    unsat_scl->weight++;
                    for (int l_sign_idx : unsat_scl->bool_literals)
                    {
                        _vars[_lits[std::abs(l_sign_idx)].delta].score++;
                    }
                }
            }
        }

        // soft
        total_clause_weight += unsat_clauses->size();
        total_clause_weight += unsat_soft_clauses->size();
    }

    void ls_solver::smooth_clause_weight()
    {
        for (int i = 0; i < _num_clauses; i++)
        {
            // if (_clauses[i].weight > 1 && !unsat_clauses->is_in_array(i))
            if (_clauses[i].weight > h_inc && !unsat_clauses->is_in_array(i))
            {
                // _clauses[i].weight--;
                // total_clause_weight--;
                _clauses[i].weight -= h_inc;
                total_clause_weight -= h_inc;
                if (_clauses[i].sat_count == 1 && !_lits[std::abs(_clauses[i].min_delta_lit_index)].is_lia_lit)
                {
                    __int128_t watch_lit_idx = _lits[std::abs(_clauses[i].min_delta_lit_index)].delta;
                    //_vars[watch_lit_idx].score++;
                    _vars[watch_lit_idx].score += h_inc;
                }
            }
        }
        // soft to be revised to fit weighted

        for (int i = 0; i < _num_soft_clauses; i++)
        {
            if (_soft_clauses[i].weight > 1 && !unsat_soft_clauses->is_in_array(i))
            {
                _soft_clauses[i].weight -= 1;
                total_clause_weight -= 1;
                if (_soft_clauses[i].sat_count == 1 && !_lits[std::abs(_soft_clauses[i].min_delta_lit_index)].is_lia_lit)
                {
                    __int128_t watch_lit_idx = _lits[std::abs(_soft_clauses[i].min_delta_lit_index)].delta;
                    _vars[watch_lit_idx].score += 1;
                }
            }
        }

        // float ave_hard_weight=0;
        // float ave_weight=0;
        // for (int i = 0; i < _num_soft_clauses; i++)
        // {
        //    ave_weight+=_soft_clauses[i].weight;
        // }
        // for(int i=0;i<_clauses.size();i++)
        // {
        //     ave_hard_weight+=_clauses[i].weight;
        // }
        // ave_weight/=_num_soft_clauses;
        // ave_hard_weight/=_clauses.size();
        // std::cout<<ave_hard_weight<<"          "<<ave_weight<<std::endl;
        // int cur_obj=0;
        // for(int i=0;i<unsat_soft_clauses->size();i++)
        // {
        //     cur_obj+=_soft_clauses[unsat_soft_clauses->element_at(i)].org_weight;
        // }
        // std::cout<<cur_obj<<std::endl;
        // soft
    }

    void ls_solver::random_walk()
    {
        int clause_idx, operation_idx, var_idx, operation_direction;
        __int128_t change_value;
        __int128_t best_subscore = INT64_MAX;
        int best_score_bool = INT32_MIN;
        __int128_t subscore;
        int score;
        int best_score = INT32_MIN;
        int best_operation_idx = 0;
        int best_operation_idx_bool = 0;
        uint64_t best_last_move = UINT64_MAX;
        uint64_t best_last_move_bool = UINT64_MAX;
        uint64_t last_move_step;
        operation_idx = 0;
        int operation_idx_bool = 0;
        clause *cp;
        lit *l;
        // determine the operations
        if (!unsat_clauses->empty())
        {
            // new add
            //  if(issat==1&&!unsat_soft_clauses->empty()&&mt()%100>97)
            //  {
            //      int best_index = -1, best_weight = -1, cur_weight=-1;
            //      for(int i=0;i<unsat_soft_clauses->size();i++)
            //      {
            //          cur_weight=_soft_clauses[unsat_soft_clauses->element_at(i)].org_weight;
            //          if(cur_weight>best_weight)
            //          {
            //              best_index=i;
            //              best_weight=cur_weight;
            //          }
            //      }
            //      soft_clause *scp =&(_soft_clauses[unsat_soft_clauses->element_at(best_index)]);
            //      for (int l_idx : scp->lia_literals)
            //      {
            //          l = &(_lits[std::abs(l_idx)]);
            //          for (int k = 0; k < l->pos_coff.size(); k++)
            //          {
            //              var_idx = l->pos_coff_var_idx[k];
            //              change_value = (l_idx > 0) ? devide(-l->delta, l->pos_coff[k]) : devide(1 - l->delta, l->pos_coff[k]);
            //              insert_operation(var_idx, change_value, operation_idx);
            //          }
            //          for (int k = 0; k < l->neg_coff.size(); k++)
            //          {
            //              var_idx = l->neg_coff_var_idx[k];
            //              change_value = (l_idx > 0) ? devide(l->delta, l->neg_coff[k]) : devide(l->delta - 1, l->neg_coff[k]);
            //              insert_operation(var_idx, change_value, operation_idx);
            //          }
            //      }
            //      for (int i = 0; i < operation_idx; i++)
            //      {
            //          var_idx = operation_var_idx_vec[i];
            //          change_value = operation_change_value_vec[i];
            //          score = critical_score(var_idx, change_value);
            //          if (score > best_score )
            //          {
            //              best_score = score;
            //              best_operation_idx = i;
            //          }
            //      }
            //      // make move
            //      if (operation_idx_bool == 0 || (operation_idx > 0 && operation_idx_bool > 0 && !is_in_bool_search))
            //      {
            //          var_idx = operation_var_idx_vec[best_operation_idx];
            //          change_value = operation_change_value_vec[best_operation_idx];
            //      }
            //      critical_move(var_idx, change_value);
            //      update_clause_weight();
            //      return ;
            //  }
            //  new add

            for (int i = 0; i < 3 && operation_idx + operation_idx_bool < 5; i++)
            // for (int i = 0; i < 1 && operation_idx + operation_idx_bool < 5; i++)
            {
                clause_idx = unsat_clauses->element_at(mt() % unsat_clauses->size());
                cp = &(_clauses[clause_idx]);
                for (int l_idx : cp->lia_literals)
                {
                    l = &(_lits[std::abs(l_idx)]);
                    if (l->is_equal)
                    {
                        if (l_idx < 0)
                        {
                            for (int var_idx : l->pos_coff_var_idx)
                            {
                                insert_operation(var_idx, 1, operation_idx);
                                insert_operation(var_idx, -1, operation_idx);
                            }
                            for (int var_idx : l->neg_coff_var_idx)
                            {
                                insert_operation(var_idx, 1, operation_idx);
                                insert_operation(var_idx, -1, operation_idx);
                            }
                        } // delta should not be 0, while it is 0, so the var should increase 1/-1
                        else
                        {
                            for (int j = 0; j < l->pos_coff.size(); j++)
                            {
                                int var_idx = l->pos_coff_var_idx[j];
                                if ((l->delta % l->pos_coff[j]) != 0)
                                {
                                    continue;
                                }
                                insert_operation(var_idx, (-l->delta / l->pos_coff[j]), operation_idx);
                            }
                            for (int j = 0; j < l->neg_coff.size(); j++)
                            {
                                int var_idx = l->neg_coff_var_idx[j];
                                if ((l->delta % l->neg_coff[j]) != 0)
                                {
                                    continue;
                                }
                                insert_operation(var_idx, (l->delta / l->neg_coff[j]), operation_idx);
                            }
                        } // delta should be 0, while it is not 0, so the var should increase (-delta/coff), while (-delta%coff)==0
                        continue;
                    }
                    for (int k = 0; k < l->pos_coff.size(); k++)
                    {
                        var_idx = l->pos_coff_var_idx[k];
                        change_value = (l_idx > 0) ? devide(-l->delta, l->pos_coff[k]) : devide(1 - l->delta, l->pos_coff[k]);
                        insert_operation(var_idx, change_value, operation_idx);
                    }
                    for (int k = 0; k < l->neg_coff.size(); k++)
                    {
                        var_idx = l->neg_coff_var_idx[k];
                        change_value = (l_idx > 0) ? devide(l->delta, l->neg_coff[k]) : devide(l->delta - 1, l->neg_coff[k]);
                        insert_operation(var_idx, change_value, operation_idx);
                    }
                }
                for (int l_idx : cp->bool_literals)
                {
                    __int128_t bool_var_idx = _lits[std::abs(l_idx)].delta;
                    if (!is_chosen_bool_var[bool_var_idx])
                    {
                        operation_var_idx_bool_vec[operation_idx_bool++] = (int)bool_var_idx;
                        is_chosen_bool_var[bool_var_idx] = true;
                    }
                }
            }
            for (int i = 0; i < operation_idx_bool; i++)
            {
                is_chosen_bool_var[operation_var_idx_bool_vec[i]] = false;
            }
            // choose the best operation
            // for (int i = 0; i < operation_idx; i++)
            // {
            //     var_idx = operation_var_idx_vec[i];
            //     change_value = operation_change_value_vec[i];
            //     score = critical_score(var_idx, change_value);
            //     //else    subscore = critical_score(var_idx, change_value);
            //     operation_direction = (change_value > 0) ? 0 : 1;
            //     if (score > best_score )
            //     {
            //         best_score = subscore;
            //         best_operation_idx = i;
            //     }
            // }
            for (int i = 0; i < operation_idx; i++)
            {
                var_idx = operation_var_idx_vec[i];
                change_value = operation_change_value_vec[i];
                subscore = critical_subscore(var_idx, change_value);
                // else    subscore = critical_score(var_idx, change_value);
                operation_direction = (change_value > 0) ? 0 : 1;
                last_move_step = last_move[2 * var_idx + (operation_direction + 1) % 2];
                if (subscore < best_subscore || (subscore == best_subscore && last_move_step < best_last_move))
                {
                    best_subscore = subscore;
                    best_last_move = last_move_step;
                    best_operation_idx = i;
                }
            }
            for (int i = 0; i < operation_idx_bool; i++)
            {
                var_idx = operation_var_idx_bool_vec[i];
                score = _vars[var_idx].score;
                uint64_t last_move_step = last_move[2 * var_idx];
                if (score > best_score_bool || (score == best_score_bool && last_move_step < best_last_move_bool))
                {
                    best_score_bool = score;
                    best_last_move_bool = last_move_step;
                    best_operation_idx_bool = i;
                }
            }
            // make move
            if (operation_idx + operation_idx_bool == 0)
            {
                return;
            }
            if (operation_idx_bool == 0 || (operation_idx > 0 && operation_idx_bool > 0 && !is_in_bool_search))
            {
                // if(mt()%10000<9999||best_subscore>0)
                // {
                var_idx = operation_var_idx_vec[best_operation_idx];
                change_value = operation_change_value_vec[best_operation_idx];
                // }
                // else
                // {
                //     int index=mt()%operation_idx;
                //     var_idx = operation_var_idx_vec[index];
                //     change_value = operation_change_value_vec[index];
                // }
            }
            else
            {
                var_idx = operation_var_idx_bool_vec[best_operation_idx_bool];
                change_value = 0;
            }
            critical_move(var_idx, change_value);
        }
        else if (!unsat_soft_clauses->empty())
        {
            // best_subscore = INT64_MAX;
            // best_score_bool = INT32_MIN;
            // best_operation_idx = 0;
            // best_operation_idx_bool = 0;
            // best_last_move = UINT64_MAX;
            // best_last_move_bool = UINT64_MAX;
            // operation_idx = 0;
            // operation_idx_bool = 0;

            // //new add
            // if(!is_in_bool_search&&issat==1&&!unsat_soft_clauses->empty()&&mt()%100>=90)
            // {
            //     int best_index = -1, best_weight = -1, cur_weight=-1;
            //     for(int i=0;i<unsat_soft_clauses->size();i++)
            //     {
            //         cur_weight=_soft_clauses[unsat_soft_clauses->element_at(i)].org_weight;
            //         if(cur_weight>best_weight)
            //         {
            //             best_index=i;
            //             best_weight=cur_weight;
            //         }
            //     }
            //     soft_clause *scp =&(_soft_clauses[unsat_soft_clauses->element_at(best_index)]);
            //     for (int l_idx : scp->lia_literals)
            //     {
            //         l = &(_lits[std::abs(l_idx)]);
            //         for (int k = 0; k < l->pos_coff.size(); k++)
            //         {
            //             var_idx = l->pos_coff_var_idx[k];
            //             change_value = (l_idx > 0) ? devide(-l->delta, l->pos_coff[k]) : devide(1 - l->delta, l->pos_coff[k]);
            //             insert_operation(var_idx, change_value, operation_idx);
            //         }
            //         for (int k = 0; k < l->neg_coff.size(); k++)
            //         {
            //             var_idx = l->neg_coff_var_idx[k];
            //             change_value = (l_idx > 0) ? devide(l->delta, l->neg_coff[k]) : devide(l->delta - 1, l->neg_coff[k]);
            //             insert_operation(var_idx, change_value, operation_idx);
            //         }
            //     }
            //     for (int i = 0; i < operation_idx; i++)
            //     {
            //         var_idx = operation_var_idx_vec[i];
            //         change_value = operation_change_value_vec[i];
            //         subscore = critical_subscore(var_idx, change_value);
            //         operation_direction = (change_value > 0) ? 0 : 1;
            //         last_move_step = last_move[2 * var_idx + (operation_direction + 1) % 2];
            //         if (subscore < best_subscore || (subscore == best_subscore && last_move_step < best_last_move))
            //         {
            //             best_subscore = subscore;
            //             best_last_move = last_move_step;
            //             best_operation_idx = i;
            //         }
            //     }
            //     // make move
            //     if (operation_idx_bool == 0 || (operation_idx > 0 && operation_idx_bool > 0 && !is_in_bool_search))
            //     {
            //         var_idx = operation_var_idx_vec[best_operation_idx];
            //         change_value = operation_change_value_vec[best_operation_idx];
            //     }
            //     critical_move(var_idx, change_value);
            //     return ;
            // }

            // new add

            soft_clause *scp;
            for (int i = 0; i < 3 && operation_idx + operation_idx_bool < 5; i++)
            {
                clause_idx = unsat_soft_clauses->element_at(mt() % unsat_soft_clauses->size());
                scp = &(_soft_clauses[clause_idx]);
                for (int l_idx : scp->lia_literals)
                {
                    l = &(_lits[std::abs(l_idx)]);
                    if (l->is_equal)
                    {
                        if (l_idx < 0)
                        {
                            for (int var_idx : l->pos_coff_var_idx)
                            {
                                insert_operation(var_idx, 1, operation_idx);
                                insert_operation(var_idx, -1, operation_idx);
                            }
                            for (int var_idx : l->neg_coff_var_idx)
                            {
                                insert_operation(var_idx, 1, operation_idx);
                                insert_operation(var_idx, -1, operation_idx);
                            }
                        } // delta should not be 0, while it is 0, so the var should increase 1/-1
                        else
                        {
                            for (int j = 0; j < l->pos_coff.size(); j++)
                            {
                                int var_idx = l->pos_coff_var_idx[j];
                                if ((l->delta % l->pos_coff[j]) != 0)
                                {
                                    continue;
                                }
                                insert_operation(var_idx, (-l->delta / l->pos_coff[j]), operation_idx);
                            }
                            for (int j = 0; j < l->neg_coff.size(); j++)
                            {
                                int var_idx = l->neg_coff_var_idx[j];
                                if ((l->delta % l->neg_coff[j]) != 0)
                                {
                                    continue;
                                }
                                insert_operation(var_idx, (l->delta / l->neg_coff[j]), operation_idx);
                            }
                        } // delta should be 0, while it is not 0, so the var should increase (-delta/coff), while (-delta%coff)==0
                        continue;
                    }
                    for (int k = 0; k < l->pos_coff.size(); k++)
                    {
                        var_idx = l->pos_coff_var_idx[k];
                        change_value = (l_idx > 0) ? devide(-l->delta, l->pos_coff[k]) : devide(1 - l->delta, l->pos_coff[k]);
                        insert_operation(var_idx, change_value, operation_idx);
                    }
                    for (int k = 0; k < l->neg_coff.size(); k++)
                    {
                        var_idx = l->neg_coff_var_idx[k];
                        change_value = (l_idx > 0) ? devide(l->delta, l->neg_coff[k]) : devide(l->delta - 1, l->neg_coff[k]);
                        insert_operation(var_idx, change_value, operation_idx);
                    }
                }
                for (int l_idx : scp->bool_literals)
                {
                    __int128_t bool_var_idx = _lits[std::abs(l_idx)].delta;
                    if (!is_chosen_bool_var[bool_var_idx])
                    {
                        operation_var_idx_bool_vec[operation_idx_bool++] = (int)bool_var_idx;
                        is_chosen_bool_var[bool_var_idx] = true;
                    }
                }
            }
            for (int i = 0; i < operation_idx_bool; i++)
            {
                is_chosen_bool_var[operation_var_idx_bool_vec[i]] = false;
            }
            // choose the best operation
            // for (int i = 0; i < operation_idx; i++)
            // {
            //     var_idx = operation_var_idx_vec[i];
            //     change_value = operation_change_value_vec[i];
            //     score = critical_score(var_idx, change_value);
            //     //else    subscore = critical_score(var_idx, change_value);
            //     operation_direction = (change_value > 0) ? 0 : 1;
            //     if (score > best_score )
            //     {
            //         best_score = subscore;
            //         best_operation_idx = i;
            //     }
            // }
            for (int i = 0; i < operation_idx; i++)
            {
                var_idx = operation_var_idx_vec[i];
                change_value = operation_change_value_vec[i];
                subscore = critical_subscore(var_idx, change_value);
                // subscore = critical_score(var_idx, change_value);
                operation_direction = (change_value > 0) ? 0 : 1;
                last_move_step = last_move[2 * var_idx + (operation_direction + 1) % 2];
                if (subscore < best_subscore || (subscore == best_subscore && last_move_step < best_last_move))
                {
                    best_subscore = subscore;
                    best_last_move = last_move_step;
                    best_operation_idx = i;
                }
            }
            for (int i = 0; i < operation_idx_bool; i++)
            {
                var_idx = operation_var_idx_bool_vec[i];
                score = _vars[var_idx].score;
                uint64_t last_move_step = last_move[2 * var_idx];
                if (score > best_score_bool || (score == best_score_bool && last_move_step < best_last_move_bool))
                {
                    best_score_bool = score;
                    best_last_move_bool = last_move_step;
                    best_operation_idx_bool = i;
                }
            }
            // make move
            if (operation_idx + operation_idx_bool == 0)
            {
                return;
            }
            if (operation_idx_bool == 0 || (operation_idx > 0 && operation_idx_bool > 0 && !is_in_bool_search))
            {
                // if(mt()%10000<9999)
                // {
                var_idx = operation_var_idx_vec[best_operation_idx];
                change_value = operation_change_value_vec[best_operation_idx];
                // }
                // else
                // {
                //     int index=mt()%operation_idx;
                //     var_idx = operation_var_idx_vec[index];
                //     change_value = operation_change_value_vec[index];
                // }
            }
            else
            {
                var_idx = operation_var_idx_bool_vec[best_operation_idx_bool];
                change_value = 0;
            }
            critical_move(var_idx, change_value);
        }
    }

    // construction
    void ls_solver::construct_slution_score()
    {
        // TODO::this is a temp function, setting all vars 0
        // std::vector<int>bool_num;
        if (issat != 0 && flag_restart == 0)
        {
        }
        else
        {
            for (int i = 0; i < _num_vars; i++)
            {
                if (!_vars[i].is_lia)
                {
                    // bool_num.push_back(i);
                    _solution[i] = 1;
                    continue;
                }
                if (_vars[i].low_bound > 0)
                // if (_vars[i].low_bound > INT32_MIN)
                {
                    _solution[i] = _vars[i].low_bound;
                }
                else if (_vars[i].upper_bound < 0)
                // else if(_vars[i].upper_bound < INT32_MAX)
                {
                    _solution[i] = _vars[i].upper_bound;
                }
                else
                {
                    _solution[i] = 0;
                }
            }
        }

        // for (int i = 0; i < _num_vars; i++)
        // std::cout<<"i= "<<print_128(_solution[i])<<std::endl;
    }

    uint64_t ls_solver::pick_construct_idx(__int128_t &best_value)
    {
        return 0;
    }

    void ls_solver::construct_move(uint64_t var_idx, __int128_t change_value)
    {
    }

    int ls_solver::construct_score(uint64_t var_idx, __int128_t change_value)
    {
        return 0;
    }

    // basic operations
    bool ls_solver::update_best_solution()
    {
        bool improve = false;
        if (unsat_clauses->size() < best_found_this_restart)
        {
            improve = true;
            best_found_this_restart = unsat_clauses->size();
        }
        if (unsat_clauses->size() < best_found_cost)
        {
            improve = true;
            best_found_cost = unsat_clauses->size();
            best_cost_time = TimeElapsed();
        }
        return improve;
    }

    void ls_solver::modify_CC(uint64_t var_idx, int direction)
    {
    }

    int ls_solver::pick_critical_two_level_bool()
    {

        two_level_triple two_level_best = two_level_find_best();
        if (two_level_best.var_idx != -1)
        {
            if (two_level_best.score > 0)
            {
                critical_move(two_level_best.var_idx, two_level_best.change_value);
                return 1;
            }
        }
        if (pick_critical_pair() == 1)
            return 1;
        return -1;
    }
    
    int ls_solver::pick_critical_move_bool()
    {

        if (pick_critical_two_level_bool() == 1)
            return -1;
        int best_score, score, best_var_idx, cnt, operation;
        bool BMS = false;
        best_score = 1;
        best_var_idx = -1;
        uint64_t best_last_move = UINT64_MAX;
        int operation_idx = 0;
        int goodvarnum = 0;
        // for(int i=0;i<bool_var_vec.size();i++)
        // {
        //     if(_vars[bool_var_vec[i]].score>0)
        //     {
        //         goodvarnum++;
        //     }
        // }
        // std::cout<<"boolsize:"<<contain_bool_unsat_clauses->size()<<"           "<<goodvarnum<<std::endl;
        for (int i = 0; i < contain_bool_unsat_clauses->size(); i++)
        {
            int clause_idx = contain_bool_unsat_clauses->element_at(i);
            clause *cl = &(_clauses[clause_idx]);
            for (int l_sign_idx : cl->bool_literals)
            {
                lit *l = &(_lits[std::abs(l_sign_idx)]);
                if (is_chosen_bool_var[l->delta])
                    continue;
                if (_step > tabulist[2 * l->delta] && CClist[2 * l->delta] > 0)
                {
                    operation_var_idx_bool_vec[operation_idx++] = (int)l->delta;
                    is_chosen_bool_var[l->delta] = true;
                }
            }
        }
        // soft
        for (int i = 0; i < contain_bool_unsat_soft_clauses->size(); i++)
        {
            int clause_idx = contain_bool_unsat_soft_clauses->element_at(i);
            soft_clause *cl = &(_soft_clauses[clause_idx]);
            for (int l_sign_idx : cl->bool_literals)
            {
                lit *l = &(_lits[std::abs(l_sign_idx)]);
                if (is_chosen_bool_var[l->delta])
                    continue;
                if (_step > tabulist[2 * l->delta] && CClist[2 * l->delta] > 0)
                {
                    operation_var_idx_bool_vec[operation_idx++] = (int)l->delta;
                    is_chosen_bool_var[l->delta] = true;
                }
            }
        }

        // soft
        for (int i = 0; i < operation_idx; i++)
        {
            is_chosen_bool_var[operation_var_idx_bool_vec[i]] = false;
        } // recover chosen_bool_var
        if (operation_idx > 100)
        {
            BMS = true;
            cnt = 100;
        }
        else
        {
            BMS = false;
            cnt = operation_idx;
        }
        for (int i = 0; i < cnt; i++)
        {
            if (BMS)
            {
                int idx = mt() % (operation_idx - i);
                int tmp = operation_var_idx_bool_vec[operation_idx - i - 1];
                operation = operation_var_idx_bool_vec[idx];
                operation_var_idx_bool_vec[idx] = tmp;
            }
            else
            {
                operation = operation_var_idx_bool_vec[i];
            }
            int var_idx = operation;
            score = _vars[var_idx].score;
            uint64_t last_move_step = last_move[2 * var_idx];
            if (score > best_score || (/*score>0&&*/ score == best_score && last_move_step < best_last_move))
            {
                best_score = score;
                best_var_idx = var_idx;
                best_last_move = last_move_step;
            }
        }
        // if there is untabu decreasing move
        if (best_var_idx != -1)
        {
            return best_var_idx;
        }
        // update weight
        if (1)
        {
            if (mt() % 10000 > smooth_probability)
            {
                update_clause_weight();
            }
            else
            {
                smooth_clause_weight();
            }
            random_walk();
        }

        return -1;
    }

    int ls_solver::pick_critical_move(__int128_t &best_value)
    {
        int best_score, score, operation_var_idx, best_var_idx, cnt;
        __int128_t operation_change_value, change_value;
        bool BMS = false;
        bool should_push_vec;
        best_score = (is_idl) ? 0 : 1;
        // best_score = INT16_MIN;
        best_var_idx = -1;
        change_value = 0;
        uint64_t best_last_move = UINT64_MAX;
        int operation_idx = 0;
        // determine the critical value
        for (int i = 0; i < unsat_clauses->size(); i++)
        {
            clause *cl = &(_clauses[unsat_clauses->element_at(i)]);
            for (int l_idx : cl->lia_literals)
            {
                lit *l = &(_lits[std::abs(l_idx)]);
                if (l->is_equal)
                {
                    if (l_idx < 0)
                    {
                        for (int var_idx : l->pos_coff_var_idx)
                        {
                            if (_step > tabulist[2 * var_idx])
                            {
                                insert_operation(var_idx, 1, operation_idx);
                            }
                            if (_step > tabulist[2 * var_idx + 1])
                            {
                                insert_operation(var_idx, -1, operation_idx);
                            }
                        }
                        for (int var_idx : l->neg_coff_var_idx)
                        {
                            if (_step > tabulist[2 * var_idx])
                            {
                                insert_operation(var_idx, 1, operation_idx);
                            }
                            if (_step > tabulist[2 * var_idx + 1])
                            {
                                insert_operation(var_idx, -1, operation_idx);
                            }
                        }
                    } // delta should not be 0, while it is 0, so the var should increase 1/-1
                    else
                    {
                        for (int j = 0; j < l->pos_coff.size(); j++)
                        {
                            int var_idx = l->pos_coff_var_idx[j];
                            if ((l->delta % l->pos_coff[j]) != 0)
                            {
                                continue;
                            }
                            if ((l->delta < 0 && _step > tabulist[2 * var_idx]) || (l->delta > 0 && _step > tabulist[2 * var_idx + 1]))
                            {
                                insert_operation(var_idx, (-l->delta / l->pos_coff[j]), operation_idx);
                            }
                        }
                        for (int j = 0; j < l->neg_coff.size(); j++)
                        {
                            int var_idx = l->neg_coff_var_idx[j];
                            if ((l->delta % l->neg_coff[j]) != 0)
                            {
                                continue;
                            }
                            if ((l->delta > 0 && _step > tabulist[2 * var_idx]) || (l->delta < 0 && _step > tabulist[2 * var_idx + 1]))
                            {
                                insert_operation(var_idx, (l->delta / l->neg_coff[j]), operation_idx);
                            }
                        }
                    } // delta should be 0, while it is not 0, so the var should increase (-delta/coff), while (-delta%coff)==0
                    continue;
                }
                for (int i = 0; i < l->pos_coff.size(); i++)
                {
                    should_push_vec = false;
                    int var_idx = l->pos_coff_var_idx[i];
                    if (var_idx == lia_var_idx_with_most_lits)
                    {
                        continue;
                    }
                    if (l_idx > 0 && _step > tabulist[2 * var_idx + 1])
                    // if (l_idx > 0)
                    {
                        should_push_vec = true;
                        change_value = devide(-l->delta, l->pos_coff[i]);
                    }
                    else if (l_idx < 0 && _step > tabulist[2 * var_idx])
                    // else if (l_idx < 0)
                    {
                        should_push_vec = true;
                        change_value = devide(1 - l->delta, l->pos_coff[i]);
                    }
                    if (should_push_vec)
                    {
                        insert_operation(var_idx, change_value, operation_idx);
                    }
                    // if l_idx>0, delta should be <=0, while it is now >0(too large), so the var should enlarge by (-delta/coff) (this is a negative value), if l_idx<0, delta should be >=1, while it is now <1(too small), so the var should enlarge by (1-delta)/coff (positive value)
                }
                for (int i = 0; i < l->neg_coff.size(); i++)
                {
                    should_push_vec = false;
                    int var_idx = l->neg_coff_var_idx[i];
                    if (var_idx == lia_var_idx_with_most_lits)
                    {
                        continue;
                    }
                    if (l_idx > 0 && _step > tabulist[2 * var_idx])
                    // if (l_idx > 0 )
                    {
                        should_push_vec = true;
                        change_value = devide(l->delta, l->neg_coff[i]);
                    }
                    else if (l_idx < 0 && _step > tabulist[2 * var_idx + 1])
                    // else if (l_idx < 0 )
                    {
                        should_push_vec = true;
                        change_value = devide(l->delta - 1, l->neg_coff[i]);
                    }
                    if (should_push_vec)
                    {
                        insert_operation(var_idx, change_value, operation_idx);
                    }
                    // if l_idx>0, delta should be <=0, while it is now >0(too large), so the var should enlarge by (delta/coff) (this is a positive value since the coff is neg), if l_idx<0, the delta should be >=1, while it is now <1(too small), so the var should enlarge by (delta-1)/coff (neg value)
                }
            }
        }
        // soft
        if (unsat_clauses->size() == 0)
        {
            for (int i = 0; i < unsat_soft_clauses->size(); i++)
            {
                soft_clause *cl = &(_soft_clauses[unsat_soft_clauses->element_at(i)]);
                for (int l_idx : cl->lia_literals)
                {
                    lit *l = &(_lits[std::abs(l_idx)]);
                    if (l->is_equal)
                    {
                        // to be addded
                    }
                    else
                    {
                        for (int i = 0; i < l->pos_coff.size(); i++)
                        {
                            should_push_vec = false;
                            int var_idx = l->pos_coff_var_idx[i];
                            if (var_idx == lia_var_idx_with_most_lits)
                            {
                                continue;
                            }
                            if (l_idx > 0 && _step > tabulist[2 * var_idx + 1])
                            {
                                should_push_vec = true;
                                change_value = devide(-l->delta, l->pos_coff[i]);
                            }
                            else if (l_idx < 0 && _step > tabulist[2 * var_idx])
                            {
                                should_push_vec = true;
                                change_value = devide(1 - l->delta, l->pos_coff[i]);
                            }
                            if (should_push_vec)
                            {
                                insert_operation(var_idx, change_value, operation_idx);
                            }
                            // if l_idx>0, delta should be <=0, while it is now >0(too large), so the var should enlarge by (-delta/coff) (this is a negative value), if l_idx<0, delta should be >=1, while it is now <1(too small), so the var should enlarge by (1-delta)/coff (positive value)
                        }
                        for (int i = 0; i < l->neg_coff.size(); i++)
                        {
                            should_push_vec = false;
                            int var_idx = l->neg_coff_var_idx[i];
                            if (var_idx == lia_var_idx_with_most_lits)
                            {
                                continue;
                            }
                            if (l_idx > 0 && _step > tabulist[2 * var_idx])
                            // if (l_idx > 0)
                            {
                                should_push_vec = true;
                                change_value = devide(l->delta, l->neg_coff[i]);
                            }
                            else if (l_idx < 0 && _step > tabulist[2 * var_idx + 1])
                            // else if (l_idx < 0 )
                            {
                                should_push_vec = true;
                                change_value = devide(l->delta - 1, l->neg_coff[i]);
                            }
                            if (should_push_vec)
                            {
                                insert_operation(var_idx, change_value, operation_idx);
                            }
                            // if l_idx>0, delta should be <=0, while it is now >0(too large), so the var should enlarge by (delta/coff) (this is a positive value since the coff is neg), if l_idx<0, the delta should be >=1, while it is now <1(too small), so the var should enlarge by (delta-1)/coff (neg value)
                        }
                    }
                }
            }
        }
        // soft
        //  go through the forward and backward move of vars, evaluate their score, pick the untabued best one
        if (operation_idx > 200)
        {
            BMS = true;
            cnt = 200;
            // BMS = false;
            // cnt = operation_idx;
        }
        else
        {
            BMS = false;
            cnt = operation_idx;
        }
        std::vector<int> goodvar;
        std::vector<__int128_t> goodv_change_value;
        std::vector<int> goodvar_score;
        for (int i = 0; i < cnt; i++)
        {
            if (BMS)
            {
                int idx = mt() % (operation_idx - i);
                operation_change_value = operation_change_value_vec[idx];
                operation_var_idx = operation_var_idx_vec[idx];
                operation_change_value_vec[idx] = operation_change_value_vec[operation_idx - i - 1];
                operation_var_idx_vec[idx] = operation_var_idx_vec[operation_idx - i - 1];
            }
            else
            {
                operation_change_value = operation_change_value_vec[i];
                operation_var_idx = operation_var_idx_vec[i];
            }
            score = critical_score(operation_var_idx, operation_change_value);
            if (score > 0)
            {
                goodvar.push_back(operation_var_idx);
                goodv_change_value.push_back(operation_change_value);
                goodvar_score.push_back(score);
            }
            int opposite_direction = (operation_change_value > 0) ? 1 : 0; // if the change value is >0, then means it is moving forward, the opposite direction is 1(backward)
            uint64_t last_move_step = last_move[2 * operation_var_idx + opposite_direction];
            if (score > best_score || (score == best_score /*&&score>0*/ && last_move_step < best_last_move))
            {
                best_score = score;
                best_var_idx = operation_var_idx;
                best_value = operation_change_value;
                best_last_move = last_move_step;
            }
        }
        if (best_var_idx != -1)
        {
            // std::cout<<best_var_idx<<"  "<<best_score<<std::endl;
            return best_var_idx;
        }
        // soft
        //  update weight and random walk
        if (0)
        {
            if (mt() % 10000 > smooth_probability)
            {
                update_clause_weight();
            }
            else
            {
                smooth_clause_weight();
            }
            random_walk();
            return -1;
        }
        else
        {

            if (pick_critical_pair() == 1)
                return -1;
            // if(0){}
            else
            {
                if (mt() % 10000 > smooth_probability)
                {
                    update_clause_weight();
                }
                else
                {
                    smooth_clause_weight();
                }
                random_walk();
                return -1;
            }
        }
    }

    int ls_solver::pick_critical_two_level()
    {
        // int softflag=0;
        // std::vector<int> sc;
        // if(unsat_clauses->size()>8)
        // {
        //     while (sc.size() < 8)
        //     {
        //         int tmp_clause = unsat_clauses->element_at(mt() % unsat_clauses->size());
        //         if (find(sc.begin(), sc.end(), tmp_clause) == sc.end())
        //         {
        //             sc.push_back(tmp_clause);
        //         }
        //     }
        // }
        // else if(unsat_clauses->size()>0)
        // {
        //     for (int i = 0; i <unsat_clauses->size(); i++)
        //     {
        //         sc.push_back(unsat_clauses->element_at(i));
        //     }
        // }
        // else if(unsat_soft_clauses->size()>15)
        // {
        //     softflag=1;
        //     while (sc.size() < 15)
        //     {
        //         int tmp_clause = unsat_soft_clauses->element_at(mt() % unsat_soft_clauses->size());
        //         if (find(sc.begin(), sc.end(), tmp_clause) == sc.end())
        //         {
        //             sc.push_back(tmp_clause);
        //         }
        //     }
        // }
        // else
        // {
        //     softflag=1;
        //     for (int i = 0; i <unsat_soft_clauses->size(); i++)
        //     {
        //         sc.push_back(unsat_soft_clauses->element_at(i));
        //     }
        // }
        // int v1;
        // __int128_t s1 = INT64_MIN;
        // __int128_t s2 = INT64_MIN;
        // std::vector<lit> FV;
        // std::vector<__int128_t>FV_change_value;
        // clause *cp;
        // soft_clause*scp;
        // int var_idx;
        // __int128_t change_value;
        // int operation_idx = 0;
        // int score;
        // std::vector<int>pre_var;
        // std::vector<__int128_t>pre_value;
        // std::vector<int>pre_score;
        // for(int i=0;i<sc.size();i++)
        // {
        //     if(softflag==0)
        //     {
        //         cp=&(_clauses[sc[i]]);
        //         // for(int i=0;i<cp->lia_literals.size();i++)
        //         // {
        //             int randl=cp->lia_literals[mt()%(cp->lia_literals.size())];
        //             //int randl=cp->lia_literals[i];
        //             lit * l = &(_lits[std::abs(randl)]);
        //             for (int k = 0; k < l->pos_coff.size(); k++)
        //             {
        //                 var_idx = l->pos_coff_var_idx[k];
        //                 change_value = (randl > 0) ? devide(-l->delta, l->pos_coff[k]) : devide(1 - l->delta, l->pos_coff[k]);
        //                 //if((randl>0&&_step<tabulist[2*var_idx+1])||(randl<0&&_step<tabulist[2*var_idx])) continue;
        //                 if(is_in_bound(var_idx,change_value)==0) continue;
        //                 // if(is_in_bound(var_idx,change_value)==true)
        //                 // {
        //                     pre_var.push_back(var_idx);
        //                     pre_value.push_back(change_value);
        //                     pre_score.push_back(critical_score(var_idx,change_value));
        //                 //}

        //             }
        //             for (int k = 0; k < l->neg_coff.size(); k++)
        //             {

        //                 var_idx = l->neg_coff_var_idx[k];
        //                 change_value = (randl > 0) ? devide(l->delta, l->neg_coff[k]) : devide(l->delta - 1, l->neg_coff[k]);
        //                 //if((randl>0&&_step<tabulist[2*var_idx])||(randl<0&&_step<tabulist[2*var_idx+1])) continue;
        //                 if(is_in_bound(var_idx,change_value)==0) continue;
        //                 // if(is_in_bound(var_idx,change_value)==true)
        //                 // {
        //                     pre_var.push_back(var_idx);
        //                     pre_value.push_back(change_value);
        //                     pre_score.push_back(critical_score(var_idx,change_value));
        //                 //}
        //             }
        //         //}

        //     }
        //     else
        //     {
        //         scp=&(_soft_clauses[sc[i]]);
        //         int randl=scp->lia_literals[mt()%(scp->lia_literals.size())];
        //         lit * l = &(_lits[std::abs(randl)]);
        //         for (int k = 0; k < l->pos_coff.size(); k++)
        //         {
        //             var_idx = l->pos_coff_var_idx[k];
        //             change_value = (randl > 0) ? devide(-l->delta, l->pos_coff[k]) : devide(1 - l->delta, l->pos_coff[k]);
        //             //if((randl>0&&_step<tabulist[2*var_idx+1])||(randl<0&&_step<tabulist[2*var_idx])) continue;
        //             if(is_in_bound(var_idx,change_value)==0) continue;
        //             pre_var.push_back(var_idx);
        //             pre_value.push_back(change_value);
        //             pre_score.push_back(critical_score(var_idx,change_value));
        //         }
        //         for (int k = 0; k < l->neg_coff.size(); k++)
        //         {
        //             var_idx = l->neg_coff_var_idx[k];
        //             change_value = (randl > 0) ? devide(l->delta, l->neg_coff[k]) : devide(l->delta - 1, l->neg_coff[k]);
        //             //if((randl>0&&_step<tabulist[2*var_idx])||(randl<0&&_step<tabulist[2*var_idx+1])) continue;
        //             if(is_in_bound(var_idx,change_value)==0) continue;
        //             pre_var.push_back(var_idx);
        //             pre_value.push_back(change_value);
        //             pre_score.push_back(critical_score(var_idx,change_value));
        //         }
        //     }
        // }
        // // std::cout<<"two"<<std::endl;
        // int best_first_level_score=INT16_MIN;
        // int best_idx=-1;
        // //std::cout<<pre_score.size()<<std::endl;
        // for(int i=0;i<pre_score.size();i++)
        // {
        //     if(pre_score[i]>best_first_level_score)
        //     {
        //         best_first_level_score=pre_score[i];
        //         best_idx=i;
        //     }
        // }
        // // if(best_idx!=-1&&pre_score[best_idx]>0)
        // // {
        // //     critical_move(pre_var[best_idx],pre_value[best_idx]);
        // //     update_clause_weight();
        // //     return 1;
        // // }
        // // for(int i=0;i<pre_var.size();i++) std::cout<<pre_var[i]<<"  "<<print_128(pre_value[i])<<"   "<<pre_score[i]<<"   "<<std::endl;
        // int best_score=INT16_MIN,best_var_1=-1,best_var_2;
        // __int128_t best_change_1,best_change_2;
        // for(int i=0;i<pre_var.size();i++)
        // {
        //     //move to be added
        //     critical_move_pse(pre_var[i],pre_value[i]);
        //     two_level_triple two_level_best=two_level_find_best();
        //     if(two_level_best.var_idx!=-1)
        //     {
        //         if(pre_score[i]+two_level_best.score>0&&(abs(pre_value[i])==abs(two_level_best.change_value)))
        //         {
        //             std::cout<<pre_score[i]+two_level_best.score<<"  "<<print_128(pre_value[i])<<" "<<two_level_best.var_idx<<" "<<print_128(two_level_best.change_value)<<std::endl;
        //             int direction = (pre_value[i] > 0) ? 0 : 1;
        //             tabulist[pre_var[i] * 2 + (direction + 1) % 2] = _step + 3 + mt() % 10;
        //             critical_move(two_level_best.var_idx,two_level_best.change_value);
        //             update_clause_weight();
        //             // int flag=0;
        //             // for(int m=0;m<_vars[pre_var[i]].literals.size();m++)
        //             // {
        //             //     for(int n=0;n<_vars[two_level_best.var_idx].literals.size();n++)
        //             //     {
        //             //         if(abs(_vars[pre_var[i]].literals[m])==abs(_vars[two_level_best.var_idx].literals[n]))
        //             //         {
        //             //             flag+=1;
        //             //             //break;
        //             //         }
        //             //     }
        //             // }
        //             // if(flag==1)
        //             // {
        //             //     //if(pre_value[i]!=two_level_best.change_value) std::cout<<"not equal!    ";
        //             //      std::cout<<"not a lit: "<<pre_var[i]<<"  "<<print_128(pre_value[i])<<"  "<<two_level_best.var_idx<<"  "<<print_128(two_level_best.change_value)<<std::endl;
        //             // }

        //             // else
        //             // {
        //             //     if(abs(pre_value[i])!=abs(two_level_best.change_value))
        //             //     {
        //             //         std::cout<<"not equal!    ";
        //             //         std::cout<<flag<<std::endl;
        //             //     }
        //             //     std::cout<<pre_var[i]<<"  "<<print_128(pre_value[i])<<"  "<<two_level_best.var_idx<<"  "<<print_128(two_level_best.change_value)<<std::endl;
        //             // }
        //             return 1;
        //         }
        //         // if(pre_score[i]+two_level_best.score>best_score)
        //         // {
        //         //     best_score=pre_score[i]+two_level_best.score;
        //         //     best_var_1=pre_var[i];best_change_1=pre_value[i];
        //         //     best_var_2=two_level_best.var_idx;best_change_2=two_level_best.change_value;
        //         // }
        //     }
        //     critical_move_pse(pre_var[i],-pre_value[i]);
        // }
        // //std::cout<<pre_var.size()<<std::endl;
        // // if(best_idx==-1)
        // // {
        // //     std::cout<<pre_score.size()<<std::endl;
        // //     for(int i=0;i<pre_score.size();i++)
        // //     {
        // //         std::cout<<pre_score[i]<<std::endl;
        // //     }
        // // }
        // //std::cout<<best_idx<<std::endl;
        // // int best_first_level_score=INT16_MIN;
        // // int best_idx=-1;
        // // //std::cout<<pre_score.size()<<std::endl;
        // // for(int i=0;i<pre_score.size();i++)
        // // {
        // //     if(pre_score[i]>best_first_level_score)
        // //     {
        // //         best_first_level_score=pre_score[i];
        // //         best_idx=i;
        // //     }
        // // }
        // // if(best_first_level_score<best_score)
        // // {
        // //     //std::cout<<best_var_1<<std::endl;
        // //     critical_move(best_var_1,best_change_1);
        // //     update_clause_weight();
        // //     return 1;
        // // }
        // // else if(best_idx!=-1)
        // // {
        // //     std::cout<<pre_var[best_idx]<<"  "<<print_128(pre_value[best_idx])<<"   "<<pre_score[best_idx]<<std::endl;
        // //     critical_move(pre_var[best_idx],pre_value[best_idx]);
        // //     update_clause_weight();
        // //     return 1;
        // // }
        // // if(best_var_1!=-1)
        // // {
        // //     //critical_move(best_var_1,best_change_1);
        // //     critical_move(best_var_2,best_change_2);
        // //     update_clause_weight();
        // //     return 1;
        // // }
        return -1; // bool?
    }
    
    int ls_solver::two_level_find_best_bool()
    {
        int best_score, score, best_var_idx, cnt, operation;
        bool BMS = false;
        best_score = 0;
        best_var_idx = -1;
        uint64_t best_last_move = UINT64_MAX;
        int operation_idx = 0;
        for (int i = 0; i < contain_bool_unsat_clauses->size(); i++)
        {
            int clause_idx = contain_bool_unsat_clauses->element_at(i);
            clause *cl = &(_clauses[clause_idx]);
            for (int l_sign_idx : cl->bool_literals)
            {
                lit *l = &(_lits[std::abs(l_sign_idx)]);
                if (is_chosen_bool_var[l->delta])
                    continue;
                if (_step > tabulist[2 * l->delta] && CClist[2 * l->delta] > 0)
                {
                    operation_var_idx_bool_vec[operation_idx++] = (int)l->delta;
                    is_chosen_bool_var[l->delta] = true;
                }
            }
        }
        // soft
        for (int i = 0; i < contain_bool_unsat_soft_clauses->size(); i++)
        {
            int clause_idx = contain_bool_unsat_soft_clauses->element_at(i);
            soft_clause *cl = &(_soft_clauses[clause_idx]);
            for (int l_sign_idx : cl->bool_literals)
            {
                lit *l = &(_lits[std::abs(l_sign_idx)]);
                if (is_chosen_bool_var[l->delta])
                    continue;
                if (_step > tabulist[2 * l->delta] && CClist[2 * l->delta] > 0)
                {
                    operation_var_idx_bool_vec[operation_idx++] = (int)l->delta;
                    is_chosen_bool_var[l->delta] = true;
                }
            }
        }

        // soft
        for (int i = 0; i < operation_idx; i++)
        {
            is_chosen_bool_var[operation_var_idx_bool_vec[i]] = false;
        } // recover chosen_bool_var
        if (operation_idx > 45)
        {
            BMS = true;
            cnt = 45;
        }
        else
        {
            BMS = false;
            cnt = operation_idx;
        }
        for (int i = 0; i < cnt; i++)
        {
            if (BMS)
            {
                int idx = mt() % (operation_idx - i);
                int tmp = operation_var_idx_bool_vec[operation_idx - i - 1];
                operation = operation_var_idx_bool_vec[idx];
                operation_var_idx_bool_vec[idx] = tmp;
            }
            else
            {
                operation = operation_var_idx_bool_vec[i];
            }
            int var_idx = operation;
            score = _vars[var_idx].score;
            uint64_t last_move_step = last_move[2 * var_idx];
            if (score > best_score || (score > 0 && score == best_score && last_move_step < best_last_move))
            {
                best_score = score;
                best_var_idx = var_idx;
                best_last_move = last_move_step;
            }
        }
        return best_var_idx;
    }
    
    two_level_triple ls_solver::two_level_find_best()
    {
        int best_score, score, operation_var_idx, best_var_idx, cnt;
        __int128_t operation_change_value, change_value, best_value;
        bool BMS = false;
        bool should_push_vec;
        best_score = (is_idl) ? 0 : 1;
        // best_score = INT16_MIN;
        best_var_idx = -1;
        change_value = 0;
        uint64_t best_last_move = UINT64_MAX;
        int operation_idx = 0;
        // determine the critical value
        for (int i = 0; i < unsat_clauses->size(); i++)
        {
            clause *cl = &(_clauses[unsat_clauses->element_at(i)]);
            for (int l_idx : cl->lia_literals)
            {
                lit *l = &(_lits[std::abs(l_idx)]);
                if (l->is_equal)
                {
                    if (l_idx < 0)
                    {
                        for (int var_idx : l->pos_coff_var_idx)
                        {
                            if (_step > tabulist[2 * var_idx])
                            {
                                insert_operation(var_idx, 1, operation_idx);
                            }
                            if (_step > tabulist[2 * var_idx + 1])
                            {
                                insert_operation(var_idx, -1, operation_idx);
                            }
                        }
                        for (int var_idx : l->neg_coff_var_idx)
                        {
                            if (_step > tabulist[2 * var_idx])
                            {
                                insert_operation(var_idx, 1, operation_idx);
                            }
                            if (_step > tabulist[2 * var_idx + 1])
                            {
                                insert_operation(var_idx, -1, operation_idx);
                            }
                        }
                    } // delta should not be 0, while it is 0, so the var should increase 1/-1
                    else
                    {
                        for (int j = 0; j < l->pos_coff.size(); j++)
                        {
                            int var_idx = l->pos_coff_var_idx[j];
                            if ((l->delta % l->pos_coff[j]) != 0)
                            {
                                continue;
                            }
                            if ((l->delta < 0 && _step > tabulist[2 * var_idx]) || (l->delta > 0 && _step > tabulist[2 * var_idx + 1]))
                            {
                                insert_operation(var_idx, (-l->delta / l->pos_coff[j]), operation_idx);
                            }
                        }
                        for (int j = 0; j < l->neg_coff.size(); j++)
                        {
                            int var_idx = l->neg_coff_var_idx[j];
                            if ((l->delta % l->neg_coff[j]) != 0)
                            {
                                continue;
                            }
                            if ((l->delta > 0 && _step > tabulist[2 * var_idx]) || (l->delta < 0 && _step > tabulist[2 * var_idx + 1]))
                            {
                                insert_operation(var_idx, (l->delta / l->neg_coff[j]), operation_idx);
                            }
                        }
                    } // delta should be 0, while it is not 0, so the var should increase (-delta/coff), while (-delta%coff)==0
                    continue;
                }
                for (int i = 0; i < l->pos_coff.size(); i++)
                {
                    should_push_vec = false;
                    int var_idx = l->pos_coff_var_idx[i];
                    if (var_idx == lia_var_idx_with_most_lits)
                    {
                        continue;
                    }
                    // if (l_idx > 0 && _step > tabulist[2 * var_idx + 1])
                    if (l_idx > 0)
                    {
                        should_push_vec = true;
                        change_value = devide(-l->delta, l->pos_coff[i]);
                    }
                    // else if (l_idx < 0 && _step > tabulist[2 * var_idx])
                    else if (l_idx < 0)
                    {
                        should_push_vec = true;
                        change_value = devide(1 - l->delta, l->pos_coff[i]);
                    }
                    if (should_push_vec)
                    {
                        insert_operation(var_idx, change_value, operation_idx);
                    }
                    // if l_idx>0, delta should be <=0, while it is now >0(too large), so the var should enlarge by (-delta/coff) (this is a negative value), if l_idx<0, delta should be >=1, while it is now <1(too small), so the var should enlarge by (1-delta)/coff (positive value)
                }
                for (int i = 0; i < l->neg_coff.size(); i++)
                {
                    should_push_vec = false;
                    int var_idx = l->neg_coff_var_idx[i];
                    if (var_idx == lia_var_idx_with_most_lits)
                    {
                        continue;
                    }
                    // if (l_idx > 0 && _step > tabulist[2 * var_idx])
                    if (l_idx > 0)
                    {
                        should_push_vec = true;
                        change_value = devide(l->delta, l->neg_coff[i]);
                    }
                    // else if (l_idx < 0 && _step > tabulist[2 * var_idx + 1])
                    else if (l_idx < 0)
                    {
                        should_push_vec = true;
                        change_value = devide(l->delta - 1, l->neg_coff[i]);
                    }
                    if (should_push_vec)
                    {
                        insert_operation(var_idx, change_value, operation_idx);
                    }
                    // if l_idx>0, delta should be <=0, while it is now >0(too large), so the var should enlarge by (delta/coff) (this is a positive value since the coff is neg), if l_idx<0, the delta should be >=1, while it is now <1(too small), so the var should enlarge by (delta-1)/coff (neg value)
                }
            }
        }
        // soft
        for (int i = 0; i < unsat_soft_clauses->size(); i++)
        {
            soft_clause *cl = &(_soft_clauses[unsat_soft_clauses->element_at(i)]);
            for (int l_idx : cl->lia_literals)
            {
                lit *l = &(_lits[std::abs(l_idx)]);
                if (l->is_equal)
                {
                    // to be addded
                }
                else
                {
                    for (int i = 0; i < l->pos_coff.size(); i++)
                    {
                        should_push_vec = false;
                        int var_idx = l->pos_coff_var_idx[i];
                        if (var_idx == lia_var_idx_with_most_lits)
                        {
                            continue;
                        }
                        // if (l_idx > 0 && _step > tabulist[2 * var_idx + 1])
                        if (l_idx > 0)
                        {
                            should_push_vec = true;
                            change_value = devide(-l->delta, l->pos_coff[i]);
                        }
                        // else if (l_idx < 0 && _step > tabulist[2 * var_idx])
                        else if (l_idx < 0)
                        {
                            should_push_vec = true;
                            change_value = devide(1 - l->delta, l->pos_coff[i]);
                        }
                        if (should_push_vec)
                        {
                            insert_operation(var_idx, change_value, operation_idx);
                        }
                        // if l_idx>0, delta should be <=0, while it is now >0(too large), so the var should enlarge by (-delta/coff) (this is a negative value), if l_idx<0, delta should be >=1, while it is now <1(too small), so the var should enlarge by (1-delta)/coff (positive value)
                    }
                    for (int i = 0; i < l->neg_coff.size(); i++)
                    {
                        should_push_vec = false;
                        int var_idx = l->neg_coff_var_idx[i];
                        if (var_idx == lia_var_idx_with_most_lits)
                        {
                            continue;
                        }
                        // if (l_idx > 0 && _step > tabulist[2 * var_idx])
                        if (l_idx > 0)
                        {
                            should_push_vec = true;
                            change_value = devide(l->delta, l->neg_coff[i]);
                        }
                        // else if (l_idx < 0 && _step > tabulist[2 * var_idx + 1])
                        else if (l_idx < 0)
                        {
                            should_push_vec = true;
                            change_value = devide(l->delta - 1, l->neg_coff[i]);
                        }
                        if (should_push_vec)
                        {
                            insert_operation(var_idx, change_value, operation_idx);
                        }
                        // if l_idx>0, delta should be <=0, while it is now >0(too large), so the var should enlarge by (delta/coff) (this is a positive value since the coff is neg), if l_idx<0, the delta should be >=1, while it is now <1(too small), so the var should enlarge by (delta-1)/coff (neg value)
                    }
                }
            }
        }
        // soft
        //  go through the forward and backward move of vars, evaluate their score, pick the untabued best one
        if (operation_idx > 150)
        {
            BMS = true;
            cnt = 150;
            // BMS = false;
            // cnt = operation_idx;
        }
        else
        {
            BMS = false;
            cnt = operation_idx;
        }
        std::vector<int> goodvar;
        std::vector<__int128_t> goodv_change_value;
        for (int i = 0; i < cnt; i++)
        {
            if (BMS)
            {
                int idx = mt() % (operation_idx - i);
                operation_change_value = operation_change_value_vec[idx];
                operation_var_idx = operation_var_idx_vec[idx];
                operation_change_value_vec[idx] = operation_change_value_vec[operation_idx - i - 1];
                operation_var_idx_vec[idx] = operation_var_idx_vec[operation_idx - i - 1];
            }
            else
            {
                operation_change_value = operation_change_value_vec[i];
                operation_var_idx = operation_var_idx_vec[i];
            }
            score = critical_score(operation_var_idx, operation_change_value);
            if (score > 0)
            {
                goodvar.push_back(operation_var_idx);
                goodv_change_value.push_back(operation_change_value);
            }
            int opposite_direction = (operation_change_value > 0) ? 1 : 0; // if the change value is >0, then means it is moving forward, the opposite direction is 1(backward)
            uint64_t last_move_step = last_move[2 * operation_var_idx + opposite_direction];
            if (score > best_score || (score == best_score /*&&score>0*/ && last_move_step < best_last_move))
            {
                best_score = score;
                best_var_idx = operation_var_idx;
                best_value = operation_change_value;
                best_last_move = last_move_step;
            }
        }
        two_level_triple best_two_level;
        if (best_var_idx != -1)
        {
            best_two_level.score = best_score;
            best_two_level.var_idx = best_var_idx;
            best_two_level.change_value = best_value;
        }
        else
        {
            best_two_level.score = -1;
            best_two_level.var_idx = -1;
            best_two_level.change_value = -1;
        }
        return best_two_level;
    }

    void ls_solver::critical_move(uint64_t var_idx, __int128_t change_value)
    {
        // std::cout<<var_idx<<" " <<print_128(change_value)<<std::endl;//print -1 max not answer
        int direction = (change_value > 0) ? 0 : 1;
        if (_vars[var_idx].is_lia)
        {
            critical_score_subscore(var_idx, change_value);
            _solution[var_idx] += change_value;
        }
        else
        {
            // std::cout<<"here"<<std::endl;
            int origin_score = _vars[var_idx].score;
            critical_score_subscore(var_idx);
            _solution[var_idx] *= -1;
            _vars[var_idx].score = -origin_score;
        }
        // step
        if (_vars[var_idx].is_lia)
        {
            last_move[2 * var_idx + direction] = _step;
            // tabulist[var_idx * 2 + (direction + 1) % 2] = _step + 3 + mt() % 10;
            if (is_weighted == 1)
                tabulist[var_idx * 2 + (direction + 1) % 2] = _step + 3 + mt() % 10;
            else
                tabulist[var_idx * 2 + (direction + 1) % 2] = _step + 3 + mt() % 10;
            if (CC_mode != -1)
            {
                modify_CC(var_idx, direction);
            }
        }
        else
        {
            last_move[2 * var_idx] = _outer_layer_step;
            tabulist[2 * var_idx] = _outer_layer_step + 1 + mt() % 3;
            if (CC_mode != -1)
            {
                modify_CC(var_idx, direction);
            }
            _outer_layer_step++;
        }
    }
    // transfer the ">" to "<="
    void ls_solver::invert_lit(lit &l)
    {
        l.key = 1 - l.key;
        std::vector<int> tmp_coff_var_idx = l.pos_coff_var_idx;
        std::vector<__int128_t> tmp_coff = l.pos_coff;
        l.pos_coff_var_idx = l.neg_coff_var_idx;
        l.pos_coff = l.neg_coff;
        l.neg_coff_var_idx = tmp_coff_var_idx;
        l.neg_coff = tmp_coff;
    }
    // all coffs are positive, go through all terms of the literal
    __int128_t ls_solver::delta_lit(lit &l)
    {
        __int128_t delta = l.key;
        for (int i = 0; i < l.pos_coff.size(); i++)
        {
            delta += (l.pos_coff[i] * _solution[l.pos_coff_var_idx[i]]);
        }
        for (int i = 0; i < l.neg_coff.size(); i++)
        {
            delta -= (l.neg_coff[i] * _solution[l.neg_coff_var_idx[i]]);
        }
        return delta;
    }

    double ls_solver::TimeElapsed()
    {
        std::chrono::steady_clock::time_point finish = std::chrono::steady_clock::now();
        std::chrono::duration<double> duration = finish - start;
        return duration.count();
    }

    void ls_solver::clear_prev_data()
    {
        for (int v : bool_var_vec)
        {
            _vars[v].score = 0;
        }
        _best_found_hard_cost_this_bool = INT32_MAX;
        _best_found_hard_cost_this_lia = INT32_MAX;
        no_improve_cnt_bool = 0;
        no_improve_cnt_lia = 0;
    }
    // return the upper round of (a/b): (-3.5)->-4; (3.5)->4
    __int128_t ls_solver::devide(__int128_t a, __int128_t b)
    {
        __int128_t up_round = (abs_128(a)) / (b);
        if (abs_128(a) % b > 0)
        {
            up_round++;
        }
        return a > 0 ? up_round : -up_round;
    }
    
    void ls_solver::insert_operation(int var_idx, __int128_t change_value, int &operation_idx)
    {
        __int128_t future_solution = _solution[var_idx] + change_value;
        bool no_pre_value = (_pre_value_1[var_idx] == INT32_MAX && _pre_value_2[var_idx] == INT32_MAX && future_solution >= _vars[var_idx].low_bound && future_solution <= _vars[var_idx].upper_bound);
        bool has_pre_value_1 = (_pre_value_1[var_idx] != INT32_MAX && _pre_value_2[var_idx] == INT32_MAX && future_solution == _pre_value_1[var_idx]);
        bool has_pre_value_2 = (_pre_value_1[var_idx] != INT32_MAX && _pre_value_2[var_idx] != INT32_MAX && (future_solution == _pre_value_1[var_idx] || future_solution == _pre_value_2[var_idx]));
        if (no_pre_value || has_pre_value_1 || has_pre_value_2)
        // if (1)
        {
            //    if(future_solution>=_vars[var_idx].low_bound&&future_solution<=_vars[var_idx].upper_bound){
            operation_var_idx_vec[operation_idx] = var_idx;
            operation_change_value_vec[operation_idx++] = change_value;
        }
    }
    
    int ls_solver::is_in_bound(int var_idx, __int128_t change_value)
    {
        __int128_t future_solution = _solution[var_idx] + change_value;
        bool no_pre_value = (_pre_value_1[var_idx] == INT32_MAX && _pre_value_2[var_idx] == INT32_MAX && future_solution >= _vars[var_idx].low_bound && future_solution <= _vars[var_idx].upper_bound);
        bool has_pre_value_1 = (_pre_value_1[var_idx] != INT32_MAX && _pre_value_2[var_idx] == INT32_MAX && future_solution == _pre_value_1[var_idx]);
        bool has_pre_value_2 = (_pre_value_1[var_idx] != INT32_MAX && _pre_value_2[var_idx] != INT32_MAX && (future_solution == _pre_value_1[var_idx] || future_solution == _pre_value_2[var_idx]));
        if (no_pre_value || has_pre_value_1 || has_pre_value_2)
        {
            //    if(future_solution>=_vars[var_idx].low_bound&&future_solution<=_vars[var_idx].upper_bound){
            return 1;
        }
        return 0;
    }

    void ls_solver::add_swap_operation(int &operation_idx)
    {
        int soft_add_flag = 0;
        // if(!sat_clause_with_false_literal->empty()&&!sat_soft_clause_with_false_literal->empty()&&mt()%100000<20000||(sat_clause_with_false_literal->empty()&&!sat_soft_clause_with_false_literal->empty()))
        // {
        //     soft_add_flag=1;
        // }
        // if(!sat_soft_clause_with_false_literal->empty())
        // {
        //     if(sat_clause_with_false_literal->empty())
        //     {
        //         soft_add_flag=1;
        //     }
        //     else /*if(mt()%10000<10000)*/
        //     {
        //         soft_add_flag=1;
        //     }
        // }
        if (sat_soft_clause_with_false_literal->empty() && soft_add_flag == 1)
            std::cout << "error";
        if (sat_clause_with_false_literal->empty() && soft_add_flag == 0)
            std::cout << "error1";
        if (soft_add_flag == 0)
        {
            int clause_idx = sat_clause_with_false_literal->element_at(mt() % sat_clause_with_false_literal->size());
            clause *cl = &(_clauses[clause_idx]);
            lit *l;
            int var_idx;
            __int128_t change_value = 0;
            for (int l_idx : cl->lia_literals)
            {
                l = &(_lits[std::abs(l_idx)]);
                if (l->is_equal)
                {
                    if (l->delta == 0 && l_idx < 0)
                    {
                        for (int var_idx : l->pos_coff_var_idx)
                        {
                            insert_operation(var_idx, 1, operation_idx);
                            insert_operation(var_idx, -1, operation_idx);
                        }
                        for (int var_idx : l->neg_coff_var_idx)
                        {
                            insert_operation(var_idx, 1, operation_idx);
                            insert_operation(var_idx, -1, operation_idx);
                        }
                    } // delta should not be 0, while it is 0, so the var should increase 1/-1
                    else if (l->delta != 0 && l_idx > 0)
                    {
                        for (int j = 0; j < l->pos_coff.size(); j++)
                        {
                            int var_idx = l->pos_coff_var_idx[j];
                            if ((l->delta % l->pos_coff[j]) != 0)
                            {
                                continue;
                            }
                            insert_operation(var_idx, (-l->delta / l->pos_coff[j]), operation_idx);
                        }
                        for (int j = 0; j < l->neg_coff.size(); j++)
                        {
                            int var_idx = l->neg_coff_var_idx[j];
                            if ((l->delta % l->neg_coff[j]) != 0)
                            {
                                continue;
                            }
                            insert_operation(var_idx, (l->delta / l->neg_coff[j]), operation_idx);
                        }
                    } // delta should be 0, while it is not 0, so the var should increase (-delta/coff), while (-delta%coff)==0
                }
                else if ((l->delta > 0 && l_idx > 0) || (l->delta <= 0 && l_idx < 0))
                { // determine a false literal
                    for (int i = 0; i < l->neg_coff.size(); i++)
                    {
                        var_idx = l->neg_coff_var_idx[i];
                        if (l_idx > 0)
                        {
                            change_value = devide(l->delta, l->neg_coff[i]);
                        } // delta should <=0, while it is now >0, it should enlarge by (-delta/-coff) pos
                        else
                        {
                            change_value = devide(l->delta - 1, l->neg_coff[i]);
                        } // delta should >=1, while it is now <=0, it should enlarge by (1-delta/-coff) neg
                        insert_operation(var_idx, change_value, operation_idx);
                    }
                    for (int i = 0; i < l->pos_coff.size(); i++)
                    {
                        var_idx = l->pos_coff_var_idx[i];
                        if (l_idx > 0)
                        {
                            change_value = devide(-l->delta, l->pos_coff[i]);
                        } // delta should <=0, while it is now >0, it should enlarge by (-delta/coff) neg
                        else
                        {
                            change_value = devide(1 - l->delta, l->pos_coff[i]);
                        }                                                       // delta should >=1, while it is now <=0, it should enlarge by (1-delta/coff) pos
                        insert_operation(var_idx, change_value, operation_idx); // do not consider tabu here
                    }
                }
            }
        }
        // soft
        else
        {
            int clause_idx = sat_soft_clause_with_false_literal->element_at(mt() % sat_soft_clause_with_false_literal->size());
            soft_clause *cl = &(_soft_clauses[clause_idx]);
            lit *l;
            int var_idx;
            __int128_t change_value = 0;
            for (int l_idx : cl->lia_literals)
            {
                l = &(_lits[std::abs(l_idx)]);
                if (l->is_equal)
                {
                    if (l->delta == 0 && l_idx < 0)
                    {
                        for (int var_idx : l->pos_coff_var_idx)
                        {
                            insert_operation(var_idx, 1, operation_idx);
                            insert_operation(var_idx, -1, operation_idx);
                        }
                        for (int var_idx : l->neg_coff_var_idx)
                        {
                            insert_operation(var_idx, 1, operation_idx);
                            insert_operation(var_idx, -1, operation_idx);
                        }
                    } // delta should not be 0, while it is 0, so the var should increase 1/-1
                    else if (l->delta != 0 && l_idx > 0)
                    {
                        for (int j = 0; j < l->pos_coff.size(); j++)
                        {
                            int var_idx = l->pos_coff_var_idx[j];
                            if ((l->delta % l->pos_coff[j]) != 0)
                            {
                                continue;
                            }
                            insert_operation(var_idx, (-l->delta / l->pos_coff[j]), operation_idx);
                        }
                        for (int j = 0; j < l->neg_coff.size(); j++)
                        {
                            int var_idx = l->neg_coff_var_idx[j];
                            if ((l->delta % l->neg_coff[j]) != 0)
                            {
                                continue;
                            }
                            insert_operation(var_idx, (l->delta / l->neg_coff[j]), operation_idx);
                        }
                    } // delta should be 0, while it is not 0, so the var should increase (-delta/coff), while (-delta%coff)==0
                }
                else if ((l->delta > 0 && l_idx > 0) || (l->delta <= 0 && l_idx < 0))
                { // determine a false literal
                    for (int i = 0; i < l->neg_coff.size(); i++)
                    {
                        var_idx = l->neg_coff_var_idx[i];
                        if (l_idx > 0)
                        {
                            change_value = devide(l->delta, l->neg_coff[i]);
                        } // delta should <=0, while it is now >0, it should enlarge by (-delta/-coff) pos
                        else
                        {
                            change_value = devide(l->delta - 1, l->neg_coff[i]);
                        } // delta should >=1, while it is now <=0, it should enlarge by (1-delta/-coff) neg
                        insert_operation(var_idx, change_value, operation_idx);
                    }
                    for (int i = 0; i < l->pos_coff.size(); i++)
                    {
                        var_idx = l->pos_coff_var_idx[i];
                        if (l_idx > 0)
                        {
                            change_value = devide(-l->delta, l->pos_coff[i]);
                        } // delta should <=0, while it is now >0, it should enlarge by (-delta/coff) neg
                        else
                        {
                            change_value = devide(1 - l->delta, l->pos_coff[i]);
                        }                                                       // delta should >=1, while it is now <=0, it should enlarge by (1-delta/coff) pos
                        insert_operation(var_idx, change_value, operation_idx); // do not consider tabu here
                    }
                }
            }
        }
        // soft
    }
    // choose a clause with small weight, then choose a random lit, select the operation with greatest score in the lit
    void ls_solver::swap_from_small_weight_clause()
    {
        uint64_t min_weight = UINT64_MAX;
        uint64_t min_weight_clause_idx = 0;
        uint64_t max_weight = INT64_MIN;
        uint64_t max_weight_clause_idx = 0;
        int best_score = INT32_MIN;
        int best_operation_var = 0;
        __int128_t best_operation_value = 0;
        int score;
        __int128_t value;
        if (sat_clause_with_false_literal->size() != 0)
        {
            for (int i = 0; i < 45; i++)
            {
                int clause_idx = sat_clause_with_false_literal->element_at(mt() % sat_clause_with_false_literal->size());
                if (_clauses[clause_idx].weight < min_weight)
                {
                    min_weight = _clauses[clause_idx].weight;
                    min_weight_clause_idx = clause_idx;
                }
            }
            clause *cl = &(_clauses[min_weight_clause_idx]);
            for (int lit_sign : cl->literals)
            {
                lit *l = &(_lits[std::abs(lit_sign)]);
                __int128_t pos_delta = l->delta;
                convert_to_pos_delta(pos_delta, lit_sign);
                if (pos_delta > 0)
                { // determine a false literal
                    if (l->is_equal)
                    {
                        if (l->delta == 0 && lit_sign < 0)
                        {
                            for (int var_idx : l->pos_coff_var_idx)
                            {
                                if (_solution[var_idx] + 1 <= _vars[var_idx].upper_bound && _solution[var_idx] + 1 >= _vars[var_idx].low_bound)
                                {
                                    score = critical_score(var_idx, 1);
                                    if (score > best_score)
                                    {
                                        best_score = score;
                                        best_operation_var = var_idx;
                                        best_operation_value = 1;
                                    }
                                }
                                if (_solution[var_idx] - 1 <= _vars[var_idx].upper_bound && _solution[var_idx] - 1 >= _vars[var_idx].low_bound)
                                {
                                    score = critical_score(var_idx, -1);
                                    if (score > best_score)
                                    {
                                        best_score = score;
                                        best_operation_var = var_idx;
                                        best_operation_value = -1;
                                    }
                                }
                            }
                            for (int var_idx : l->neg_coff_var_idx)
                            {
                                if (_solution[var_idx] + 1 <= _vars[var_idx].upper_bound && _solution[var_idx] + 1 >= _vars[var_idx].low_bound)
                                {
                                    score = critical_score(var_idx, 1);
                                    if (score > best_score)
                                    {
                                        best_score = score;
                                        best_operation_var = var_idx;
                                        best_operation_value = 1;
                                    }
                                }
                                if (_solution[var_idx] - 1 <= _vars[var_idx].upper_bound && _solution[var_idx] - 1 >= _vars[var_idx].low_bound)
                                {
                                    score = critical_score(var_idx, -1);
                                    if (score > best_score)
                                    {
                                        best_score = score;
                                        best_operation_var = var_idx;
                                        best_operation_value = -1;
                                    }
                                }
                            }
                        } // delta should not be 0, while it is 0, so the var should increase 1/-1
                        else if (l->delta != 0 && lit_sign > 0)
                        {
                            for (int j = 0; j < l->pos_coff.size(); j++)
                            {
                                int var_idx = l->pos_coff_var_idx[j];
                                if ((l->delta % l->pos_coff[j]) != 0)
                                {
                                    continue;
                                }
                                value = (-l->delta / l->pos_coff[j]);
                                if (_solution[var_idx] + value <= _vars[var_idx].upper_bound && _solution[var_idx] + value >= _vars[var_idx].low_bound)
                                {
                                    score = critical_score(var_idx, value);
                                    if (score > best_score)
                                    {
                                        best_score = score;
                                        best_operation_var = var_idx;
                                        best_operation_value = value;
                                    }
                                }
                            }
                            for (int j = 0; j < l->neg_coff.size(); j++)
                            {
                                int var_idx = l->neg_coff_var_idx[j];
                                if ((l->delta % l->neg_coff[j]) != 0)
                                {
                                    continue;
                                }
                                value = (l->delta / l->neg_coff[j]);
                                if (_solution[var_idx] + value <= _vars[var_idx].upper_bound && _solution[var_idx] + value >= _vars[var_idx].low_bound)
                                {
                                    score = critical_score(var_idx, value);
                                    if (score > best_score)
                                    {
                                        best_score = score;
                                        best_operation_var = var_idx;
                                        best_operation_value = value;
                                    }
                                }
                            }
                        } // delta should be 0, while it is not 0, so the var should increase (-delta/coff), while (-delta%coff)==0
                        critical_move(best_operation_var, best_operation_value);
                    } // equal lit
                    else if (l->is_lia_lit)
                    {
                        for (int i = 0; i < l->neg_coff.size(); i++)
                        {
                            int var_idx = l->neg_coff_var_idx[i];
                            if (lit_sign > 0)
                            {
                                value = devide(l->delta, l->neg_coff[i]);
                            } // delta should <=0, while it is now >0, it should enlarge by (-delta/-coff) pos
                            else
                            {
                                value = devide(l->delta - 1, l->neg_coff[i]);
                            } // delta should >=1, while it is now <=0, it should enlarge by (1-delta/-coff) neg
                            if (_solution[var_idx] + value <= _vars[var_idx].upper_bound && _solution[var_idx] + value >= _vars[var_idx].low_bound)
                            {
                                score = critical_score(var_idx, value);
                                if (score > best_score)
                                {
                                    best_score = score;
                                    best_operation_var = var_idx;
                                    best_operation_value = value;
                                }
                            }
                        }
                        for (int i = 0; i < l->pos_coff.size(); i++)
                        {
                            int var_idx = l->pos_coff_var_idx[i];
                            if (lit_sign > 0)
                            {
                                value = devide(-l->delta, l->pos_coff[i]);
                            } // delta should <=0, while it is now >0, it should enlarge by (-delta/coff) neg
                            else
                            {
                                value = devide(1 - l->delta, l->pos_coff[i]);
                            } // delta should >=1, while it is now <=0, it should enlarge by (1-delta/coff) pos
                            if (_solution[var_idx] + value <= _vars[var_idx].upper_bound && _solution[var_idx] + value >= _vars[var_idx].low_bound)
                            {
                                score = critical_score(var_idx, value);
                                if (score > best_score)
                                {
                                    best_score = score;
                                    best_operation_var = var_idx;
                                    best_operation_value = value;
                                }
                            } // do not consider tabu here
                        }
                        critical_move(best_operation_var, best_operation_value);
                    } // a-b+k<=0
                    else
                    {
                        critical_move(l->delta, 0);
                    } // a boolean operation
                    break;
                }
            }
        }
        // soft
        //  else if(sat_soft_clause_with_false_literal->size()!=0)
        //  {
        //      for (int i = 0; i < 45; i++)
        //      {
        //          int clause_idx = sat_soft_clause_with_false_literal->element_at(mt() % sat_soft_clause_with_false_literal->size());
        //          if (_soft_clauses[clause_idx].weight > max_weight)
        //          {
        //              max_weight = _clauses[clause_idx].weight;
        //              max_weight_clause_idx = clause_idx;
        //          }
        //      }
        //      soft_clause *cl = &(_soft_clauses[max_weight_clause_idx]);
        //      for (int lit_sign : cl->literals)
        //      {
        //          lit *l = &(_lits[std::abs(lit_sign)]);
        //          __int128_t pos_delta = l->delta;
        //          convert_to_pos_delta(pos_delta, lit_sign);
        //          if (pos_delta > 0)
        //          { // determine a false literal
        //              if (l->is_equal)
        //              {
        //                  if (l->delta == 0 && lit_sign < 0)
        //                  {
        //                      for (int var_idx : l->pos_coff_var_idx)
        //                      {
        //                          if (_solution[var_idx] + 1 <= _vars[var_idx].upper_bound && _solution[var_idx] + 1 >= _vars[var_idx].low_bound)
        //                          {
        //                              score = critical_score(var_idx, 1);
        //                              if (score > best_score)
        //                              {
        //                                  best_score = score;
        //                                  best_operation_var = var_idx;
        //                                  best_operation_value = 1;
        //                              }
        //                          }
        //                          if (_solution[var_idx] - 1 <= _vars[var_idx].upper_bound && _solution[var_idx] - 1 >= _vars[var_idx].low_bound)
        //                          {
        //                              score = critical_score(var_idx, -1);
        //                              if (score > best_score)
        //                              {
        //                                  best_score = score;
        //                                  best_operation_var = var_idx;
        //                                  best_operation_value = -1;
        //                              }
        //                          }
        //                      }
        //                      for (int var_idx : l->neg_coff_var_idx)
        //                      {
        //                          if (_solution[var_idx] + 1 <= _vars[var_idx].upper_bound && _solution[var_idx] + 1 >= _vars[var_idx].low_bound)
        //                          {
        //                              score = critical_score(var_idx, 1);
        //                              if (score > best_score)
        //                              {
        //                                  best_score = score;
        //                                  best_operation_var = var_idx;
        //                                  best_operation_value = 1;
        //                              }
        //                          }
        //                          if (_solution[var_idx] - 1 <= _vars[var_idx].upper_bound && _solution[var_idx] - 1 >= _vars[var_idx].low_bound)
        //                          {
        //                              score = critical_score(var_idx, -1);
        //                              if (score > best_score)
        //                              {
        //                                  best_score = score;
        //                                  best_operation_var = var_idx;
        //                                  best_operation_value = -1;
        //                              }
        //                          }
        //                      }
        //                  } // delta should not be 0, while it is 0, so the var should increase 1/-1
        //                  else if (l->delta != 0 && lit_sign > 0)
        //                  {
        //                      for (int j = 0; j < l->pos_coff.size(); j++)
        //                      {
        //                          int var_idx = l->pos_coff_var_idx[j];
        //                          if ((l->delta % l->pos_coff[j]) != 0)
        //                          {
        //                              continue;
        //                          }
        //                          value = (-l->delta / l->pos_coff[j]);
        //                          if (_solution[var_idx] + value <= _vars[var_idx].upper_bound && _solution[var_idx] + value >= _vars[var_idx].low_bound)
        //                          {
        //                              score = critical_score(var_idx, value);
        //                              if (score > best_score)
        //                              {
        //                                  best_score = score;
        //                                  best_operation_var = var_idx;
        //                                  best_operation_value = value;
        //                              }
        //                          }
        //                      }
        //                      for (int j = 0; j < l->neg_coff.size(); j++)
        //                      {
        //                          int var_idx = l->neg_coff_var_idx[j];
        //                          if ((l->delta % l->neg_coff[j]) != 0)
        //                          {
        //                              continue;
        //                          }
        //                          value = (l->delta / l->neg_coff[j]);
        //                          if (_solution[var_idx] + value <= _vars[var_idx].upper_bound && _solution[var_idx] + value >= _vars[var_idx].low_bound)
        //                          {
        //                              score = critical_score(var_idx, value);
        //                              if (score > best_score)
        //                              {
        //                                  best_score = score;
        //                                  best_operation_var = var_idx;
        //                                  best_operation_value = value;
        //                              }
        //                          }
        //                      }
        //                  } // delta should be 0, while it is not 0, so the var should increase (-delta/coff), while (-delta%coff)==0
        //                  critical_move(best_operation_var, best_operation_value);
        //              } // equal lit
        //              else if (l->is_lia_lit)
        //              {
        //                  for (int i = 0; i < l->neg_coff.size(); i++)
        //                  {
        //                      int var_idx = l->neg_coff_var_idx[i];
        //                      if (lit_sign > 0)
        //                      {
        //                          value = devide(l->delta, l->neg_coff[i]);
        //                      } // delta should <=0, while it is now >0, it should enlarge by (-delta/-coff) pos
        //                      else
        //                      {
        //                          value = devide(l->delta - 1, l->neg_coff[i]);
        //                      } // delta should >=1, while it is now <=0, it should enlarge by (1-delta/-coff) neg
        //                      if (_solution[var_idx] + value <= _vars[var_idx].upper_bound && _solution[var_idx] + value >= _vars[var_idx].low_bound)
        //                      {
        //                          score = critical_score(var_idx, value);
        //                          if (score > best_score)
        //                          {
        //                              best_score = score;
        //                              best_operation_var = var_idx;
        //                              best_operation_value = value;
        //                          }
        //                      }
        //                  }
        //                  for (int i = 0; i < l->pos_coff.size(); i++)
        //                  {
        //                      int var_idx = l->pos_coff_var_idx[i];
        //                      if (lit_sign > 0)
        //                      {
        //                          value = devide(-l->delta, l->pos_coff[i]);
        //                      } // delta should <=0, while it is now >0, it should enlarge by (-delta/coff) neg
        //                      else
        //                      {
        //                          value = devide(1 - l->delta, l->pos_coff[i]);
        //                      } // delta should >=1, while it is now <=0, it should enlarge by (1-delta/coff) pos
        //                      if (_solution[var_idx] + value <= _vars[var_idx].upper_bound && _solution[var_idx] + value >= _vars[var_idx].low_bound)
        //                      {
        //                          score = critical_score(var_idx, value);
        //                          if (score > best_score)
        //                          {
        //                              best_score = score;
        //                              best_operation_var = var_idx;
        //                              best_operation_value = value;
        //                          }
        //                      } // do not consider tabu here
        //                  }
        //                  // std::cout<<best_operation_var<<" "<<print_128(best_operation_value)<<std::endl;
        //                  critical_move(best_operation_var, best_operation_value);
        //              } // a-b+k<=0
        //              else
        //              {
        //                  // std::cout<<"here"<<std::endl;
        //                  critical_move(l->delta, 0);
        //              } // a boolean operation
        //              break;
        //          }
        //      }
        //  }//soft
    }
    // print
    void ls_solver::print_formula()
    {
        for (int i = 0; i < _num_clauses; i++)
        {
            clause *cl = &(_clauses[i]);
            std::cout << i << "\n";
            for (int l_idx : cl->literals)
            {
                if (l_idx < 0)
                {
                    std::cout << "neg: ";
                }
                print_literal(_lits[std::abs(l_idx)]);
            }
            std::cout << "\n";
        }
    }

    void ls_solver::print_formula_pbs()
    {
        std::cout << "p wcnf " << _num_vars << " " << (_num_lits - _num_vars * 2) << " " << _num_vars + 1 << "\n";
        for (int lit_idx = 1; lit_idx < _num_lits; lit_idx++)
        {
            lit *l = &(_lits[lit_idx]);
            if (l->pos_coff.size() == 1 && l->neg_coff.size() == 0 && l->pos_coff[0] == 1 && l->key == -1)
            {
                continue;
            }
            if (l->neg_coff.size() == 1 && l->pos_coff.size() == 0 && l->neg_coff[0] == 1 && l->key == 0)
            {
                continue;
            }
            if (l->lits_index == 0)
            {
                continue;
            }
            print_lit_pbs(_lits[lit_idx]);
        }
        std::cout << "2 0 1 1 0\n";
    }

    void ls_solver::print_formula_smt()
    {
        std::cout << "(set-info :smt-lib-version 2.6)\n(set-logic QF_IDL)\n";
        for (variable &v : _vars)
        {
            std::cout << "(declare-fun " << v.var_name << " () " << (v.is_lia ? "Int" : "Bool") << ")\n";
        }
        std::cout << "(assert\n(and\n";
        for (clause &cl : _clauses)
        {
            std::cout << "(or ";
            for (int lit_idx : cl.literals)
            {
                print_lit_smt(lit_idx);
            }
            std::cout << ")\n";
        }
        std::cout << "))\n(check-sat)\n(exit)\n";
    }

    void ls_solver::print_literal(lit &l)
    {
        if (!l.is_lia_lit)
        {
            // std::cout << _vars[l.delta].var_name << "\n";
        }
        else
        {
            for (int i = 0; i < l.pos_coff.size(); i++)
            {
                std::cout << "( " << print_128(l.pos_coff[i]) << " * " << l.pos_coff_var_idx[i] << " ) + ";
            }
            for (int i = 0; i < l.neg_coff.size(); i++)
            {
                std::cout << "( -" << print_128(l.neg_coff[i]) << " * " << l.neg_coff_var_idx[i] << " ) + ";
            }
            std::cout << "( " << print_128(l.key) << " ) " << (l.is_equal ? "==" : "<=") << " 0\n";
        }
        // if (!l.is_lia_lit)
        // {
        //     std::cout << _resolution_vars[l.delta].var_name << "\n";
        // }
        // else
        // {
        //     for (int i = 0; i < l.pos_coff.size(); i++)
        //     {
        //         std::cout << "( " << print_128(l.pos_coff[i]) << " * " << _tmp_vars[l.pos_coff_var_idx[i]].var_name << " ) + ";
        //     }
        //     for (int i = 0; i < l.neg_coff.size(); i++)
        //     {
        //         std::cout << "( -" << print_128(l.neg_coff[i]) << " * " << _tmp_vars[l.neg_coff_var_idx[i]].var_name << " ) + ";
        //     }
        //     std::cout << "( " << print_128(l.key) << " ) " << (l.is_equal ? "==" : "<=") << " 0\n";
        // }
    }

    void ls_solver::print_lit_pbs(lit &l)
    {
        __int128_t degree = l.key;
        for (int i = 0; i < l.pos_coff.size(); i++)
        {
            degree += l.pos_coff[i];
        }
        std::cout << _num_vars + 1 << " " << print_128(degree) << " ";
        for (int i = 0; i < l.pos_coff.size(); i++)
        {
            std::cout << print_128(l.pos_coff[i]) << " " << -(l.pos_coff_var_idx[i] + 1) << " ";
        }
        for (int i = 0; i < l.neg_coff.size(); i++)
        {
            std::cout << print_128(l.neg_coff[i]) << " " << l.neg_coff_var_idx[i] + 1 << " ";
        }
        std::cout << "0\n";
    }

    void ls_solver::print_lit_smt(int lit_idx)
    {
        lit *l = &(_lits[std::abs(lit_idx)]);
        if (l->is_lia_lit)
        {
            std::cout << "(" << (lit_idx > 0 ? "<=" : ">") << " (- " << _vars[l->pos_coff_var_idx[0]].var_name << " " << _vars[l->neg_coff_var_idx[0]].var_name << ") " << print_128(-l->key) << " ) ";
        }
        else
        {
            if (lit_idx > 0)
            {
                std::cout << _vars[l->delta].var_name << " ";
            }
            else
            {
                std::cout << "("
                          << " not " << _vars[l->delta].var_name << " ) ";
            }
        }
    }

    void ls_solver::print_mv()
    {
        std::cout << "(model\n";
        for (uint64_t var_idx = 0; var_idx < _num_vars; var_idx++)
        {
            print_mv_vars(var_idx);
        }
        std::cout << ")\n";
    }

    void ls_solver::print_mv_vars(uint64_t var_idx)
    {
        variable *v = &(_vars[var_idx]);
        __int128_t var_solution = _best_solutin[var_idx];
        std::cout << "  (define-fun " << v->var_name << " () ";
        if (v->is_lia)
        {
            std::cout << "Int ";
            if (var_solution >= 0)
            {
                std::cout << print_128(var_solution) << ")\n";
            }
            else
            {
                std::cout << "(- " << print_128(-var_solution) << "))\n";
            }
        }
        else
        {
            std::cout << "Bool ";
            if (var_solution > 0)
            {
                std::cout << "true )\n";
            }
            else
            {
                std::cout << "false )\n";
            }
        }
    }

    void ls_solver::choose_value_for_pair()
    {
        pair_x_value.reserve(pair_x->size());
        pair_y_value.reserve(pair_x->size());
        variable *original_var_x;
        variable *original_var_y;
        __int128_t z_value, upper, lower, upper_y, lower_y, new_x_value;
        for (int pair_idx = 0; pair_idx < pair_x->size(); pair_idx++)
        {
            original_var_x = &(_tmp_vars[pair_x->element_at(pair_idx)]);
            original_var_y = &(_tmp_vars[pair_y->element_at(pair_idx)]);
            z_value = _solution[name2var["_new_var_" + original_var_x->var_name]];
            upper_y = (original_var_y->upper_bound == max_int) ? max_int : (original_var_y->upper_bound + z_value);
            lower_y = (original_var_y->low_bound == -max_int) ? (-max_int) : (original_var_y->low_bound + z_value);
            upper = (original_var_x->upper_bound < upper_y) ? original_var_x->upper_bound : upper_y;
            lower = (original_var_x->low_bound > lower_y) ? original_var_x->low_bound : lower_y;
            if (upper != max_int)
            {
                new_x_value = upper;
            }
            else if (lower != -max_int)
            {
                new_x_value = lower;
            }
            else
            {
                new_x_value = 0;
            }
            pair_x_value.push_back(new_x_value);
            pair_y_value.push_back(new_x_value - z_value); // x-y=z  x=y+z  x [0,inf) y+z [-1,inf)-->x \wedge y+z [0,inf) -->x=0--> y=x-z
        }
    }
    void ls_solver::print_var_solution(std::string &var_name, std::string &var_value)
    {
        uint64_t var_idx = 0;
        if (name2tmp_var.find(var_name) == name2tmp_var.end())
        {
            var_value = print_128(0);
            return;
        } // Bool case: since the Bool var will directly enter resolution var
        // LIA case follows
        int origin_var_idx = (int)name2tmp_var[var_name];
        if (pair_x->is_in_array(origin_var_idx))
        { // x-y=x case x
            var_value = print_128(pair_x_value[pair_x->index_of(origin_var_idx)]);
            return;
        }
        else if (pair_y->is_in_array(origin_var_idx))
        { // x-y=z case y
            var_value = print_128(pair_y_value[pair_y->index_of(origin_var_idx)]);
            return;
        }
        else if (name2var.find(var_name) != name2var.end())
        {
            var_idx = name2var[var_name];
            var_value = print_128(_solution[var_idx]);
            return;
        } // the var exists in _vars
        else
        {
            var_value = print_128(_tmp_vars[origin_var_idx].low_bound);
            return;
        } // the var does not exist in reduced formula
    }

    // calculate score
    int ls_solver::critical_score(uint64_t var_idx, __int128_t change_value)
    {
        lit *l;
        clause *cp;
        soft_clause *scp;
        int critical_score = 0;
        __int128_t delta_old, delta_new, l_clause_idx;
        // number of make_lits in a clause
        int make_break_in_clause = 0;
        variable *var = &(_vars[var_idx]);
        for (int i = 0; i < var->literals.size(); i++)
        {
            l = &(_lits[std::abs(var->literals[i])]);
            l_clause_idx = var->literal_clause[i];
            delta_old = l->delta;
            delta_new = delta_old + (var->literal_coff[i] * change_value); // l_clause_idx means that the coff is positive, and vice versa
            if ((!l->is_equal && delta_old <= 0 && delta_new > 0) || (l->is_equal && delta_old == 0 && delta_new != 0))
                make_break_in_clause = (var->literals[i] > 0) ? (make_break_in_clause - 1) : (make_break_in_clause + 1);
            else if ((!l->is_equal && delta_old > 0 && delta_new <= 0) || (l->is_equal && delta_old != 0 && delta_new == 0))
                make_break_in_clause = (var->literals[i] > 0) ? (make_break_in_clause + 1) : (make_break_in_clause - 1);
            // enter a new clause or the last literal
            if ((i != (var->literals.size() - 1) && l_clause_idx != var->literal_clause[i + 1]) || i == (var->literals.size() - 1))
            {
                cp = &(_clauses[abs_128(l_clause_idx)]);
                if (cp->sat_count == 0 && cp->sat_count + make_break_in_clause > 0)
                    critical_score += cp->weight;
                else if (cp->sat_count > 0 && cp->sat_count + make_break_in_clause == 0)
                    critical_score -= cp->weight;
                make_break_in_clause = 0;
            }
        }
        // soft
        make_break_in_clause = 0;
        for (int i = 0; i < var->soft_literals.size(); i++)
        {
            l = &(_lits[std::abs(var->soft_literals[i])]);
            l_clause_idx = var->soft_literal_clause[i];
            delta_old = l->delta;
            delta_new = delta_old + (var->soft_literal_coff[i] * change_value); // l_clause_idx means that the coff is positive, and vice versa
            if ((!l->is_equal && delta_old <= 0 && delta_new > 0) || (l->is_equal && delta_old == 0 && delta_new != 0))
                make_break_in_clause = (var->soft_literals[i] > 0) ? (make_break_in_clause - 1) : (make_break_in_clause + 1);
            else if ((!l->is_equal && delta_old > 0 && delta_new <= 0) || (l->is_equal && delta_old != 0 && delta_new == 0))
                make_break_in_clause = (var->soft_literals[i] > 0) ? (make_break_in_clause + 1) : (make_break_in_clause - 1);
            // enter a new clause or the last literal
            if ((i != (var->soft_literals.size() - 1) && l_clause_idx != var->soft_literal_clause[i + 1]) || i == (var->soft_literals.size() - 1))
            {
                scp = &(_soft_clauses[abs_128(l_clause_idx)]);
                if (scp->sat_count == 0 && scp->sat_count + make_break_in_clause > 0)
                    critical_score += scp->weight;
                else if (scp->sat_count > 0 && scp->sat_count + make_break_in_clause == 0)
                    critical_score -= scp->weight;
                make_break_in_clause = 0;
            }
        }

        // soft
        return critical_score;
    }

    void ls_solver::convert_to_pos_delta(__int128_t &delta, int l_idx)
    {
        lit *l = &(_lits[std::abs(l_idx)]);
        if (l->is_lia_lit)
        {
            if (l->is_equal)
            {
                if ((delta == 0 && l_idx > 0) || (delta != 0 && l_idx < 0))
                {
                    delta = 0;
                }
                else
                {
                    delta = 1;
                }
            }
            else
            {
                if (l_idx < 0)
                {
                    delta = 1 - delta;
                }
                if (delta < 0)
                {
                    delta = 0;
                }
            }
        }
        else
        {
            __int128_t var_idx = l->delta;
            delta = ((_solution[var_idx] > 0 && l_idx > 0) || (_solution[var_idx] < 0 && l_idx < 0)) ? 0 : 1;
        }
    }

    __int128_t ls_solver::critical_subscore(uint64_t var_idx, __int128_t change_value)
    {
        __int128_t critical_subscore = 0;
        __int128_t delta_old, delta_new;
        variable *var = &(_vars[var_idx]);
        int lit_idx, l_clause_idx;
        // the future min delta containing var
        __int128_t new_future_min_delta = INT64_MAX;
        bool contained_in_min_delta_lit = false; // determing if the var appears in the lit with min delta
        lit_occur->clear();
        for (int i = 0; i < var->literals.size(); i++)
        {
            lit_idx = var->literals[i];
            l_clause_idx = var->literal_clause[i];
            lit_occur->insert_element(std::abs(lit_idx));
            if (lit_idx == _clauses[l_clause_idx].min_delta_lit_index)
            {
                contained_in_min_delta_lit = true;
            }
            delta_new = _lits[std::abs(lit_idx)].delta + (change_value * var->literal_coff[i]);
            convert_to_pos_delta(delta_new, lit_idx);
            if (delta_new < new_future_min_delta)
            {
                new_future_min_delta = delta_new;
            }
            // enter a new clause or the last literal
            if ((i != (var->literals.size() - 1) && l_clause_idx != var->literal_clause[i + 1]) || i == (var->literals.size() - 1))
            {
                clause *cp = &(_clauses[l_clause_idx]);
                if (new_future_min_delta <= cp->min_delta)
                {
                    critical_subscore += (new_future_min_delta - cp->min_delta) * cp->weight;
                }
                else if (contained_in_min_delta_lit)
                {
                    for (int lit_idx_in_cp : cp->literals)
                    {
                        if (!lit_occur->is_in_array(std::abs(lit_idx_in_cp)))
                        {
                            delta_old = _lits[std::abs(lit_idx_in_cp)].delta;
                            convert_to_pos_delta(delta_old, lit_idx_in_cp);
                            if (delta_old < new_future_min_delta)
                            {
                                new_future_min_delta = delta_old;
                            }
                        }
                    }
                    critical_subscore += (new_future_min_delta - cp->min_delta) * cp->weight;
                } // if new_future_min_delta>cp->min_delta and var_idx belongs to the min_delta_lit
                new_future_min_delta = INT64_MAX;
                contained_in_min_delta_lit = false;
                lit_occur->clear();
            }
        }

        // soft
        lit_occur->clear();
        for (int i = 0; i < var->soft_literals.size(); i++)
        {
            lit_idx = var->soft_literals[i];
            l_clause_idx = var->soft_literal_clause[i];
            lit_occur->insert_element(std::abs(lit_idx));
            if (lit_idx == _soft_clauses[l_clause_idx].min_delta_lit_index)
            {
                contained_in_min_delta_lit = true;
            }
            delta_new = _lits[std::abs(lit_idx)].delta + (change_value * var->soft_literal_coff[i]);
            convert_to_pos_delta(delta_new, lit_idx);
            if (delta_new < new_future_min_delta)
            {
                new_future_min_delta = delta_new;
            }
            // enter a new clause or the last literal
            if ((i != (var->soft_literals.size() - 1) && l_clause_idx != var->soft_literal_clause[i + 1]) || i == (var->soft_literals.size() - 1))
            {
                soft_clause *scp = &(_soft_clauses[l_clause_idx]);
                if (new_future_min_delta <= scp->min_delta)
                {
                    critical_subscore += (new_future_min_delta - scp->min_delta) * scp->weight;
                }
                else if (contained_in_min_delta_lit)
                {
                    for (int lit_idx_in_cp : scp->literals)
                    {
                        if (!lit_occur->is_in_array(std::abs(lit_idx_in_cp)))
                        {
                            delta_old = _lits[std::abs(lit_idx_in_cp)].delta;
                            convert_to_pos_delta(delta_old, lit_idx_in_cp);
                            if (delta_old < new_future_min_delta)
                            {
                                new_future_min_delta = delta_old;
                            }
                        }
                    }
                    critical_subscore += (new_future_min_delta - scp->min_delta) * scp->weight;
                } // if new_future_min_delta>cp->min_delta and var_idx belongs to the min_delta_lit
                new_future_min_delta = INT64_MAX;
                contained_in_min_delta_lit = false;
                lit_occur->clear();
            }
        }

        // soft
        return critical_subscore;
    }
    // sat or unsat a clause, update the delta, dedicated for lia var
    void ls_solver::critical_score_subscore(uint64_t var_idx, __int128_t change_value)
    {
        static std::vector<int> lit_exist(_num_lits + _additional_len, 0);
        variable *var = &(_vars[var_idx]);
        lit *l;
        clause *cp;
        soft_clause *scp; // soft
        __int128_t l_clause_idx, delta_old, delta_new, curr_clause_idx;
        __int128_t pos_delta;
        __int128_t new_future_min_delta = INT64_MAX;
        int min_delta_lit_idx = -1;
        bool contained_in_min_delta_lit = false;
        int lit_idx;
        lit_occur->clear();
        int make_break_in_clause = 0;
        for (int i = 0; i < var->literals.size(); i++)
        {
            lit_idx = var->literals[i];
            l = &(_lits[std::abs(lit_idx)]);
            l_clause_idx = var->literal_clause[i];
            delta_old = l->delta;
            pos_delta = delta_new = (l->delta + var->literal_coff[i] * change_value);
            convert_to_pos_delta(pos_delta, lit_idx);
            if (pos_delta < new_future_min_delta)
            {
                new_future_min_delta = pos_delta;
                min_delta_lit_idx = lit_idx;
            }
            lit_occur->insert_element(std::abs(lit_idx));
            if (lit_idx == _clauses[l_clause_idx].min_delta_lit_index)
            {
                contained_in_min_delta_lit = true;
            }
            bool is_equal = l->is_equal;
            if ((!is_equal && delta_old <= 0 && delta_new > 0) || (is_equal && delta_old == 0 && delta_new != 0))
            {
                make_break_in_clause = (var->literals[i] > 0) ? (make_break_in_clause - 1) : (make_break_in_clause + 1);
            }
            else if ((!is_equal && delta_old > 0 && delta_new <= 0) || (is_equal && delta_old != 0 && delta_new == 0))
            {
                make_break_in_clause = (var->literals[i] > 0) ? (make_break_in_clause + 1) : (make_break_in_clause - 1);
            }
            // enter a new clause or the last literal
            if ((i != (var->literals.size() - 1) && l_clause_idx != var->literal_clause[i + 1]) || i == (var->literals.size() - 1))
            {
                curr_clause_idx = abs_128(l_clause_idx);
                cp = &(_clauses[curr_clause_idx]);
                if (cp->sat_count > 0 && cp->sat_count + make_break_in_clause == 0)
                {
                    unsat_a_clause(curr_clause_idx); // unsat clause
                    _lit_in_unsat_clause_num += cp->literals.size();
                    _bool_lit_in_unsat_clause_num += cp->bool_literals.size();
                }
                else if (cp->sat_count == 0 && cp->sat_count + make_break_in_clause > 0)
                {
                    sat_a_clause(curr_clause_idx); // sat a clause
                    _lit_in_unsat_clause_num -= cp->literals.size();
                    _bool_lit_in_unsat_clause_num -= cp->bool_literals.size();
                }
                int origin_sat_count = cp->sat_count;
                int origin_watch_lit = cp->min_delta_lit_index;
                cp->sat_count += make_break_in_clause;
                if (cp->sat_count > 0 && cp->sat_count < cp->literals.size())
                {
                    sat_clause_with_false_literal->insert_element((int)curr_clause_idx);
                }
                else
                {
                    sat_clause_with_false_literal->delete_element((int)curr_clause_idx);
                }
                // if new_future_min_delta<=cp->min_delta, then min_delta and watch needs updating if var is changed
                if (new_future_min_delta <= cp->min_delta)
                {
                    cp->min_delta = new_future_min_delta;
                    cp->min_delta_lit_index = min_delta_lit_idx;
                }
                else if (contained_in_min_delta_lit)
                {
                    for (int cp_lit_idx : cp->literals)
                    {
                        if (!lit_occur->is_in_array(std::abs(cp_lit_idx))) //????
                        {
                            pos_delta = _lits[std::abs(cp_lit_idx)].delta;
                            convert_to_pos_delta(pos_delta, cp_lit_idx);
                            if (pos_delta < new_future_min_delta)
                            {
                                new_future_min_delta = pos_delta; // future 已经是最小的了
                                min_delta_lit_idx = cp_lit_idx;
                            }
                        }
                    }
                    cp->min_delta = new_future_min_delta;
                    cp->min_delta_lit_index = min_delta_lit_idx;
                } // if new_future_min_delta>cp->min_delta and var_idx belongs to the watch_lit
                if (_num_bool_lits > 0)
                {
                    if (make_break_in_clause > 0)
                    {
                        if (origin_sat_count == 0)
                        {
                            for (int l_sign_idx : cp->bool_literals)
                            {
                                lit *l = &(_lits[std::abs(l_sign_idx)]);
                                _vars[l->delta].score -= cp->weight;
                            }
                        }
                        else if (origin_sat_count == 1)
                        {
                            lit *l = &(_lits[std::abs(origin_watch_lit)]);
                            if (!l->is_lia_lit)
                            {
                                _vars[l->delta].score += cp->weight;
                            }
                        }
                    }
                    else if (make_break_in_clause < 0)
                    {
                        if (cp->sat_count == 0)
                        {
                            for (int l_sign_idx : cp->bool_literals)
                            {
                                lit *l = &(_lits[std::abs(l_sign_idx)]);
                                _vars[l->delta].score += cp->weight;
                            }
                        }
                        else if (cp->sat_count == 1)
                        {
                            lit *l = &(_lits[std::abs(cp->min_delta_lit_index)]);
                            if (!l->is_lia_lit)
                            {
                                _vars[l->delta].score -= cp->weight;
                            }
                        }
                    }
                }
                make_break_in_clause = 0;
                new_future_min_delta = INT64_MAX;
                contained_in_min_delta_lit = false;
                lit_occur->clear(); //?
            }
        }
        // for (int i = 0; i < var->literals.size(); i++)
        // {
        //     lit_idx = std::abs(var->literals[i]);
        //     if (lit_exist[lit_idx] == 0)
        //     {
        //         l = &(_lits[lit_idx]);
        //         l->delta += (var->literal_coff[i] * change_value);
        //         lit_exist[lit_idx] = 1;
        //     }
        // }
        // for (int i = 0; i < var->literals.size(); i++)
        // {
        //     lit_idx = std::abs(var->literals[i]);
        //     lit_exist[lit_idx] = 0;
        // }

        // soft
        make_break_in_clause = 0;
        for (int i = 0; i < var->soft_literals.size(); i++)
        {
            lit_idx = var->soft_literals[i];
            l = &(_lits[std::abs(lit_idx)]);
            l_clause_idx = var->soft_literal_clause[i];
            delta_old = l->delta;
            pos_delta = delta_new = (l->delta + var->soft_literal_coff[i] * change_value);
            convert_to_pos_delta(pos_delta, lit_idx);
            if (pos_delta < new_future_min_delta)
            {
                new_future_min_delta = pos_delta;
                min_delta_lit_idx = lit_idx;
            }
            lit_occur->insert_element(std::abs(lit_idx));
            if (lit_idx == _soft_clauses[l_clause_idx].min_delta_lit_index)
            {
                contained_in_min_delta_lit = true;
            }
            bool is_equal = l->is_equal;
            if ((!is_equal && delta_old <= 0 && delta_new > 0) || (is_equal && delta_old == 0 && delta_new != 0))
            {
                make_break_in_clause = (var->soft_literals[i] > 0) ? (make_break_in_clause - 1) : (make_break_in_clause + 1);
            }
            else if ((!is_equal && delta_old > 0 && delta_new <= 0) || (is_equal && delta_old != 0 && delta_new == 0))
            {
                make_break_in_clause = (var->soft_literals[i] > 0) ? (make_break_in_clause + 1) : (make_break_in_clause - 1);
            }
            // enter a new clause or the last literal
            if ((i != (var->soft_literals.size() - 1) && l_clause_idx != var->soft_literal_clause[i + 1]) || i == (var->soft_literals.size() - 1))
            {
                curr_clause_idx = abs_128(l_clause_idx);
                scp = &(_soft_clauses[curr_clause_idx]);
                if (scp->sat_count > 0 && scp->sat_count + make_break_in_clause == 0)
                {
                    unsat_a_soft_clause(curr_clause_idx); // unsat clause
                    _lit_in_unsat_soft_clause_num += scp->literals.size();
                    _bool_lit_in_unsat_soft_clause_num += scp->bool_literals.size();
                }
                else if (scp->sat_count == 0 && scp->sat_count + make_break_in_clause > 0)
                {
                    sat_a_soft_clause(curr_clause_idx); // sat a clause
                    _lit_in_unsat_soft_clause_num -= scp->literals.size();
                    _bool_lit_in_unsat_soft_clause_num -= scp->bool_literals.size();
                }
                int origin_sat_count = scp->sat_count;
                int origin_watch_lit = scp->min_delta_lit_index;
                scp->sat_count += make_break_in_clause;
                if (scp->sat_count > 0 && scp->sat_count < scp->literals.size())
                {
                    sat_soft_clause_with_false_literal->insert_element((int)curr_clause_idx);
                }
                else
                {
                    sat_soft_clause_with_false_literal->delete_element((int)curr_clause_idx);
                }
                // if new_future_min_delta<=cp->min_delta, then min_delta and watch needs updating if var is changed
                if (new_future_min_delta <= scp->min_delta)
                {
                    scp->min_delta = new_future_min_delta;
                    scp->min_delta_lit_index = min_delta_lit_idx;
                }
                else if (contained_in_min_delta_lit)
                {
                    for (int cp_lit_idx : scp->literals)
                    {
                        if (!lit_occur->is_in_array(std::abs(cp_lit_idx))) //????
                        {
                            pos_delta = _lits[std::abs(cp_lit_idx)].delta;
                            convert_to_pos_delta(pos_delta, cp_lit_idx);
                            if (pos_delta < new_future_min_delta)
                            {
                                new_future_min_delta = pos_delta; // future 已经是最小的了
                                min_delta_lit_idx = cp_lit_idx;
                            }
                        }
                    }
                    scp->min_delta = new_future_min_delta;
                    scp->min_delta_lit_index = min_delta_lit_idx;
                } // if new_future_min_delta>cp->min_delta and var_idx belongs to the watch_lit
                if (_num_bool_lits > 0)
                {
                    if (make_break_in_clause > 0)
                    {
                        if (origin_sat_count == 0)
                        {
                            for (int l_sign_idx : scp->bool_literals)
                            {
                                lit *l = &(_lits[std::abs(l_sign_idx)]);
                                _vars[l->delta].score -= scp->weight;
                            }
                        }
                        else if (origin_sat_count == 1)
                        {
                            lit *l = &(_lits[std::abs(origin_watch_lit)]);
                            if (!l->is_lia_lit)
                            {
                                _vars[l->delta].score += scp->weight;
                            }
                        }
                    }
                    else if (make_break_in_clause < 0)
                    {
                        if (scp->sat_count == 0)
                        {
                            for (int l_sign_idx : scp->bool_literals)
                            {
                                lit *l = &(_lits[std::abs(l_sign_idx)]);
                                _vars[l->delta].score += scp->weight;
                            }
                        }
                        else if (scp->sat_count == 1)
                        {
                            lit *l = &(_lits[std::abs(scp->min_delta_lit_index)]);
                            if (!l->is_lia_lit)
                            {
                                _vars[l->delta].score -= scp->weight;
                            }
                        }
                    }
                }
                make_break_in_clause = 0;
                new_future_min_delta = INT64_MAX;
                contained_in_min_delta_lit = false;
                lit_occur->clear(); //?
            }
        }

        for (int i = 0; i < var->literals.size(); i++)
        {
            lit_idx = std::abs(var->literals[i]);
            if (lit_exist[lit_idx] == 0)
            {
                l = &(_lits[lit_idx]);
                l->delta += (var->literal_coff[i] * change_value);
                lit_exist[lit_idx] = 1;
            }
        }
        for (int i = 0; i < var->soft_literals.size(); i++)
        {
            lit_idx = std::abs(var->soft_literals[i]);
            if (lit_exist[lit_idx] == 0)
            {
                l = &(_lits[lit_idx]);
                l->delta += (var->soft_literal_coff[i] * change_value);
                lit_exist[lit_idx] = 1;
            }
        }
        for (int i = 0; i < var->literals.size(); i++)
        {
            lit_idx = std::abs(var->literals[i]);
            lit_exist[lit_idx] = 0;
        }
        for (int i = 0; i < var->soft_literals.size(); i++)
        {
            lit_idx = std::abs(var->soft_literals[i]);
            lit_exist[lit_idx] = 0;
        }
        // soft
    }

    // dedicated for boolean var
    void ls_solver::critical_score_subscore(uint64_t var_idx)
    {
        variable *var = &(_vars[var_idx]);
        __int128_t pos_delta;
        int make_break_in_clause = 0;
        __int128_t new_future_min_delta = INT64_MAX;
        int watch_lit = 0;
        int l_sign_idx;
        lit *l;
        for (int i = 0; i < var->literals.size(); i++)
        {
            l_sign_idx = var->literals[i];
            l = &(_lits[l_sign_idx]);
            convert_to_pos_delta(pos_delta, l_sign_idx);
            if (pos_delta == 0)
            { // true to false
                make_break_in_clause = -1;
                new_future_min_delta = 1; // TODO::average_k
            }
            else
            { // false to true
                make_break_in_clause = 1;
                new_future_min_delta = 0;
            }
            watch_lit = l_sign_idx;
            int clause_idx = var->literal_clause[i];
            clause *cp = &(_clauses[clause_idx]);
            if (cp->sat_count > 0 && cp->sat_count + make_break_in_clause == 0)
            {
                unsat_a_clause(clause_idx);
                _lit_in_unsat_clause_num += cp->literals.size();
                _bool_lit_in_unsat_clause_num += cp->bool_literals.size();
            }
            else if (cp->sat_count == 0 && cp->sat_count + make_break_in_clause > 0)
            {
                sat_a_clause(clause_idx);
                _lit_in_unsat_clause_num -= cp->literals.size();
                _bool_lit_in_unsat_clause_num -= cp->bool_literals.size();
            }
            int origin_watch_lit = cp->min_delta_lit_index;
            int origin_sat_count = cp->sat_count;
            cp->sat_count += make_break_in_clause;
            if (cp->sat_count > 0 && cp->sat_count < cp->literals.size())
            {
                sat_clause_with_false_literal->insert_element(clause_idx);
            }
            else
            {
                sat_clause_with_false_literal->delete_element(clause_idx);
            }
            if (new_future_min_delta <= cp->min_delta)
            {
                cp->min_delta = new_future_min_delta;
                cp->min_delta_lit_index = watch_lit;
            }
            else if (cp->min_delta_lit_index == l_sign_idx)
            {
                for (int l_sign_idx_inner : cp->literals)
                {
                    if (cp->min_delta_lit_index != l_sign_idx_inner)
                    {
                        pos_delta = _lits[std::abs(l_sign_idx_inner)].delta;
                        convert_to_pos_delta(pos_delta, l_sign_idx_inner);
                        if (pos_delta < new_future_min_delta)
                        {
                            new_future_min_delta = pos_delta;
                            watch_lit = l_sign_idx_inner;
                        }
                    }
                }
                cp->min_delta = new_future_min_delta;
                cp->min_delta_lit_index = watch_lit;
            }
            if (make_break_in_clause > 0)
            {
                if (origin_sat_count == 0)
                {
                    for (int bl : cp->bool_literals)
                    {
                        lit *l = &(_lits[std::abs(bl)]);
                        _vars[l->delta].score -= cp->weight;
                    }
                }
                else if (origin_sat_count == 1)
                {
                    lit *l = &(_lits[std::abs(origin_watch_lit)]);
                    if (!l->is_lia_lit)
                    {
                        _vars[l->delta].score += cp->weight;
                    }
                }
            }
            else if (make_break_in_clause < 0)
            {
                if (cp->sat_count == 0)
                {
                    for (int bl : cp->bool_literals)
                    {
                        lit *l = &(_lits[std::abs(bl)]);
                        _vars[l->delta].score += cp->weight;
                    }
                }
                else if (cp->sat_count == 1)
                {
                    lit *l = &(_lits[std::abs(cp->min_delta_lit_index)]);
                    if (!l->is_lia_lit)
                    {
                        _vars[l->delta].score -= cp->weight;
                    }
                }
            }
        }

        // soft
        for (int i = 0; i < var->soft_literals.size(); i++)
        {
            l_sign_idx = var->soft_literals[i];
            l = &(_lits[l_sign_idx]);
            convert_to_pos_delta(pos_delta, l_sign_idx);
            if (pos_delta == 0)
            { // true to false
                make_break_in_clause = -1;
                new_future_min_delta = 1; // TODO::average_k
            }
            else
            { // false to true
                make_break_in_clause = 1;
                new_future_min_delta = 0;
            }
            watch_lit = l_sign_idx;
            int clause_idx = var->soft_literal_clause[i];
            soft_clause *cp = &(_soft_clauses[clause_idx]);
            if (cp->sat_count > 0 && cp->sat_count + make_break_in_clause == 0)
            {
                unsat_a_soft_clause(clause_idx);
                _lit_in_unsat_soft_clause_num += cp->literals.size();
                _bool_lit_in_unsat_soft_clause_num += cp->bool_literals.size();
            }
            else if (cp->sat_count == 0 && cp->sat_count + make_break_in_clause > 0)
            {
                sat_a_soft_clause(clause_idx);
                _lit_in_unsat_soft_clause_num -= cp->literals.size();
                _bool_lit_in_unsat_soft_clause_num -= cp->bool_literals.size();
            }
            int origin_watch_lit = cp->min_delta_lit_index;
            int origin_sat_count = cp->sat_count;
            cp->sat_count += make_break_in_clause;
            if (cp->sat_count > 0 && cp->sat_count < cp->literals.size())
            {
                sat_soft_clause_with_false_literal->insert_element(clause_idx);
            }
            else
            {
                sat_soft_clause_with_false_literal->delete_element(clause_idx);
            }
            if (new_future_min_delta <= cp->min_delta)
            {
                cp->min_delta = new_future_min_delta;
                cp->min_delta_lit_index = watch_lit;
            }
            else if (cp->min_delta_lit_index == l_sign_idx)
            {
                for (int l_sign_idx_inner : cp->literals)
                {
                    if (cp->min_delta_lit_index != l_sign_idx_inner)
                    {
                        pos_delta = _lits[std::abs(l_sign_idx_inner)].delta;
                        convert_to_pos_delta(pos_delta, l_sign_idx_inner);
                        if (pos_delta < new_future_min_delta)
                        {
                            new_future_min_delta = pos_delta;
                            watch_lit = l_sign_idx_inner;
                        }
                    }
                }
                cp->min_delta = new_future_min_delta;
                cp->min_delta_lit_index = watch_lit;
            }
            if (make_break_in_clause > 0)
            {
                if (origin_sat_count == 0)
                {
                    for (int bl : cp->bool_literals)
                    {
                        lit *l = &(_lits[std::abs(bl)]);
                        _vars[l->delta].score -= cp->weight;
                    }
                }
                else if (origin_sat_count == 1)
                {
                    lit *l = &(_lits[std::abs(origin_watch_lit)]);
                    if (!l->is_lia_lit)
                    {
                        _vars[l->delta].score += cp->weight;
                    }
                }
            }
            else if (make_break_in_clause < 0)
            {
                if (cp->sat_count == 0)
                {
                    for (int bl : cp->bool_literals)
                    {
                        lit *l = &(_lits[std::abs(bl)]);
                        _vars[l->delta].score += cp->weight;
                    }
                }
                else if (cp->sat_count == 1)
                {
                    lit *l = &(_lits[std::abs(cp->min_delta_lit_index)]);
                    if (!l->is_lia_lit)
                    {
                        _vars[l->delta].score -= cp->weight;
                    }
                }
            }
        }
        // soft
    }

    // check
    bool ls_solver::check_solution()
    {
        // clause *cp;
        // int unsat_num = 0;
        // for (uint64_t i = 0; i < _num_clauses; i++)
        // {
        //     int sat_count = 0;
        //     cp = &(_clauses[i]);
        //     for (int lit_idx : cp->literals)
        //     {
        //         __int128_t delta = delta_lit(_lits[std::abs(lit_idx)]);
        //         bool is_equal = _lits[std::abs(lit_idx)].is_equal;
        //         if (!_lits[std::abs(lit_idx)].is_lia_lit)
        //         {
        //             __int128_t var_idx = _lits[std::abs(lit_idx)].delta;
        //             if ((_solution[var_idx] > 0 && lit_idx > 0) || (_solution[var_idx] < 0 && lit_idx < 0))
        //             {
        //                 sat_count++;
        //             }
        //         }
        //         else if ((!is_equal && ((delta <= 0 && lit_idx > 0) || (delta > 0 && lit_idx < 0))) || (is_equal && ((delta == 0 && lit_idx > 0) || (delta != 0 && lit_idx < 0))))
        //         {
        //             sat_count++;
        //         }
        //     }
        //     if (sat_count != cp->sat_count)
        //     {
        //         std::cout << "sat count wrong at clause " << i << "\n";
        //     }
        //     if (sat_count == 0)
        //     {
        //         unsat_num++;
        //     }
        // }
        // for (int var_idx = 0; var_idx < _vars.size(); var_idx++)
        // {
        //     if (_solution[var_idx] > _vars[var_idx].upper_bound || _solution[var_idx] < _vars[var_idx].low_bound)
        //     {
        //         std::cout << "var " << var_idx << " out of range\n";
        //     }
        // }
        // if (unsat_num == unsat_clauses->size())
        //     std::cout << "right solution\n";
        // else
        //     std::cout << "wrong solution\n";

        // soft
        soft_clause *cp;
        int unsat_num = 0;
        for (uint64_t i = 0; i < _soft_clauses.size(); i++)
        {
            int sat_count = 0;
            cp = &(_soft_clauses[i]);
            for (int lit_idx : cp->literals)
            {
                __int128_t delta = delta_lit(_lits[std::abs(lit_idx)]);
                bool is_equal = _lits[std::abs(lit_idx)].is_equal;
                if (!_lits[std::abs(lit_idx)].is_lia_lit)
                {
                    __int128_t var_idx = _lits[std::abs(lit_idx)].delta;
                    if ((_solution[var_idx] > 0 && lit_idx > 0) || (_solution[var_idx] < 0 && lit_idx < 0))
                    {
                        sat_count++;
                    }
                }
                else if ((!is_equal && ((delta <= 0 && lit_idx > 0) || (delta > 0 && lit_idx < 0))) || (is_equal && ((delta == 0 && lit_idx > 0) || (delta != 0 && lit_idx < 0))))
                {
                    sat_count++;
                }
            }
            if (sat_count != cp->sat_count)
            {
                std::cout << "sat count wrong at clause " << i << "\n";
            }
            if (sat_count == 0)
            {
                unsat_num++;
            }
        }
        if (unsat_num == unsat_soft_clauses->size())
            std::cout << "right solution\n";
        else
            std::cout << "wrong solution\n";

        // soft
        return unsat_num == unsat_soft_clauses->size();
    }

    bool ls_solver::update_inner_best_solution()
    {
        if (unsat_clauses->size() < _best_found_hard_cost_this_lia)
        {
            _best_found_hard_cost_this_lia = unsat_clauses->size();
            return true;
        }
        return false;
    }

    bool ls_solver::update_outer_best_solution()
    {
        if (unsat_clauses->size() < _best_found_hard_cost_this_bool)
        {
            _best_found_hard_cost_this_bool = unsat_clauses->size();
            return true;
        }
        return false;
    }

    bool ls_solver::update_best_obj()
    {
        if (0 == unsat_clauses->size())
        {
            for (int i = 0; i < _num_vars; i++)
            {
                _best_solutin[i] = _solution[i];
            }
            int cur_obj = 0;
            for (int i = 0; i < unsat_soft_clauses->size(); i++)
            {
                cur_obj += _soft_clauses[unsat_soft_clauses->element_at(i)].org_weight;
            }
            if (cur_obj < best_found_obj)
            {
                // std::cout<<cur_obj<<std::endl;
                best_time = TimeElapsed();
                best_found_obj = cur_obj;
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            std::cout << "error" << std::endl;
            return false;
        }
    }

    void ls_solver::enter_lia_mode()
    {
        _best_found_hard_cost_this_lia = unsat_clauses->size();
        no_improve_cnt_lia = 0;
        is_in_bool_search = false;
    }

    void ls_solver::enter_bool_mode()
    {
        _best_found_hard_cost_this_bool = unsat_clauses->size();
        no_improve_cnt_bool = 0;
        is_in_bool_search = true;
    }

    // local search
    int ls_solver::local_search()
    {
        // std::cout<<"bool:"<<bool_var_vec.size()<<std::endl;
        // std::cout<<_vars.size()<<std::endl;
        // for(int i=0;i<_num_lits;i++)
        //    print_literal(_lits[i]);
        int no_improve_cnt = 0;
        int flipv;
        int satnum = 0;
        int max_no_flip = 60000;
        __int128_t change_value = 0;
        start = std::chrono::steady_clock::now();
        initialize();
        _outer_layer_step = 1;
        for (_step = 1; _step < _max_step; _step++)
        {
            int isimprove = 0;
            int cur_obj = 0;
            if (0 == unsat_clauses->size() && best_found_obj == 0)
            {
                // std::cout<<_step<<std::endl;
                std::cout << best_time << std::endl;
                // std::cout<<(int)best_time<<std::endl;
                return 0;
            }
            if (0 == unsat_clauses->size() && satnum == 0)
            {
                satnum++;
                initialize(); // restart
            }
            change_value = 0;
            if (_step % 1000 == 0 && (TimeElapsed() > _cutoff))
            {
                break;
            }
            // if (no_improve_cnt > 100000&&issat==1)//60000
            // {
            //     flag_restart=1;
            //     restart();
            //     no_improve_cnt = 0;
            //     max_no_flip=150000;
            // } // restart
            if (!use_swap_from_from_small_weight || mt() % 100 < 100 || (sat_clause_with_false_literal->size() == 0 && sat_soft_clause_with_false_literal->size() == 0))
            { // only when use_swap and 1% probabilty and |sat_clauses_with_false_literal| is more than 1, do the swap from small weight

                bool time_up_bool = (no_improve_cnt_bool * _lit_in_unsat_clause_num > 5 * _bool_lit_in_unsat_clause_num);
                bool time_up_lia = (no_improve_cnt_lia * (_lit_in_unsat_clause_num + _lit_in_unsat_soft_clause_num) > 20 * (_bool_lit_in_unsat_clause_num + _bool_lit_in_unsat_soft_clause_num));
                if ((is_in_bool_search && (_bool_lit_in_unsat_clause_num + _bool_lit_in_unsat_soft_clause_num < _lit_in_unsat_clause_num + _lit_in_unsat_soft_clause_num) && time_up_bool) || (_bool_lit_in_unsat_clause_num + _bool_lit_in_unsat_soft_clause_num == 0))
                {
                    enter_lia_mode();
                }
                else if ((!is_in_bool_search && (_bool_lit_in_unsat_clause_num + _bool_lit_in_unsat_soft_clause_num > 0) && time_up_lia) || (_lit_in_unsat_clause_num + _lit_in_unsat_soft_clause_num == _bool_lit_in_unsat_clause_num + _bool_lit_in_unsat_soft_clause_num))
                {
                    enter_bool_mode();
                }
                if (is_in_bool_search)
                {
                    // std::cout<<"bool"<<std::endl;
                    flipv = pick_critical_move_bool();
                    if (flipv != -1)
                    {
                        critical_move(flipv, change_value);
                    }
                    if (0 == unsat_clauses->size())
                    {
                        issat = 1;
                        if (update_best_obj())
                        {
                            isimprove = 1;
                            no_improve_cnt_bool = 0;
                            // std::cout<<"no imp bool"<<no_improve_cnt<<std::endl;
                        }
                    }
                    else if (update_outer_best_solution())
                        no_improve_cnt_bool = 0;
                    else
                        no_improve_cnt_bool++;
                }
                else
                {
                    // std::cout<<"lia"<<std::endl;
                    flipv = pick_critical_move(change_value);
                    // flipv = pick_critical_move_dist(change_value);
                    if (flipv != -1)
                    {
                        critical_move(flipv, change_value);
                    }
                    if (0 == unsat_clauses->size())
                    {
                        issat = 1;
                        if (update_best_obj())
                        {
                            isimprove = 1;
                            no_improve_cnt_lia = 0;
                            // std::cout<<"no imp lia"<<no_improve_cnt<<std::endl;
                        }
                    }
                    else if (update_inner_best_solution())
                        no_improve_cnt_lia = 0;
                    else
                        no_improve_cnt_lia++;
                }
            }
            else
            {
                // std::cout<<"here1"<<std::endl;
                swap_from_small_weight_clause();
                if (0 == unsat_clauses->size())
                {
                    issat = 1;
                    firstsat += 2;
                    if (update_best_obj())
                    {
                        isimprove = 1;
                    }
                }
            }
            if (isimprove == 1)
            {
                no_improve_cnt = 0;
            }
            else
                no_improve_cnt++;
            // else no_improve_cnt = (update_best_solution()) ? 0 : (no_improve_cnt + 1);
        }
        // std::cout<<_step<<std::endl;
        // std::cout << best_time << std::endl;
        // std::cout<<(int)best_time<<std::endl;
        if (issat == 1)
        {
            std::cout<<" sat " <<std::endl;
            std::cout << best_time << std::endl;
            std::cout<<(best_found_obj +init_bound) <<std::endl;
        }
        else
        {
            std::cout<<"unknown";
        }
        return 1;
    }

    std::string ls_solver::print_128(__int128_t n)
    {
        std::stringstream ss;
        if (n == 0)
            return "0";
        if (n < 0)
        {
            ss << "-";
            n = -n;
        }
        int a[50], ai = 0;
        memset(a, 0, sizeof a);
        while (n != 0)
        {
            a[ai++] = n % 10;
            n /= 10;
        }
        for (int i = 1; i <= ai; i++)
            ss << abs(a[ai - i]);
        return ss.str();
    }
    
    void ls_solver::initial_bound(std::string &instring)
    {
        // std::cout<<instring<<std::endl;
        instring.erase(instring.begin());
        std::vector<std::string> vec;
        split_string(instring, vec);
        // std::cout<<vec[3];
        // init_bound = stoi(vec[3]) + reduced_bound;
        init_bound = reduced_bound;
        // std::cout<<init_bound<<std::endl;
    }
    
    int ls_solver::pick_critical_move_dist(__int128_t &best_value)
    {
        int best_score, score, operation_var_idx, best_var_idx, cnt;
        __int128_t operation_change_value, change_value;
        bool BMS = false;
        bool should_push_vec;
        best_score = (is_idl) ? 0 : 1;
        // best_score = INT16_MIN;
        best_var_idx = -1;
        change_value = 0;
        uint64_t best_last_move = UINT64_MAX;
        int operation_idx = 0;
        // determine the critical value
        for (int i = 0; i < unsat_clauses->size(); i++)
        {
            clause *cl = &(_clauses[unsat_clauses->element_at(i)]);
            for (int l_idx : cl->lia_literals)
            {
                lit *l = &(_lits[std::abs(l_idx)]);
                if (l->is_equal)
                {
                    if (l_idx < 0)
                    {
                        for (int var_idx : l->pos_coff_var_idx)
                        {
                            if (_step > tabulist[2 * var_idx])
                            {
                                insert_operation(var_idx, 1, operation_idx);
                            }
                            if (_step > tabulist[2 * var_idx + 1])
                            {
                                insert_operation(var_idx, -1, operation_idx);
                            }
                        }
                        for (int var_idx : l->neg_coff_var_idx)
                        {
                            if (_step > tabulist[2 * var_idx])
                            {
                                insert_operation(var_idx, 1, operation_idx);
                            }
                            if (_step > tabulist[2 * var_idx + 1])
                            {
                                insert_operation(var_idx, -1, operation_idx);
                            }
                        }
                    } // delta should not be 0, while it is 0, so the var should increase 1/-1
                    else
                    {
                        for (int j = 0; j < l->pos_coff.size(); j++)
                        {
                            int var_idx = l->pos_coff_var_idx[j];
                            if ((l->delta % l->pos_coff[j]) != 0)
                            {
                                continue;
                            }
                            if ((l->delta < 0 && _step > tabulist[2 * var_idx]) || (l->delta > 0 && _step > tabulist[2 * var_idx + 1]))
                            {
                                insert_operation(var_idx, (-l->delta / l->pos_coff[j]), operation_idx);
                            }
                        }
                        for (int j = 0; j < l->neg_coff.size(); j++)
                        {
                            int var_idx = l->neg_coff_var_idx[j];
                            if ((l->delta % l->neg_coff[j]) != 0)
                            {
                                continue;
                            }
                            if ((l->delta > 0 && _step > tabulist[2 * var_idx]) || (l->delta < 0 && _step > tabulist[2 * var_idx + 1]))
                            {
                                insert_operation(var_idx, (l->delta / l->neg_coff[j]), operation_idx);
                            }
                        }
                    } // delta should be 0, while it is not 0, so the var should increase (-delta/coff), while (-delta%coff)==0
                    continue;
                }
                for (int i = 0; i < l->pos_coff.size(); i++)
                {
                    should_push_vec = false;
                    int var_idx = l->pos_coff_var_idx[i];
                    if (var_idx == lia_var_idx_with_most_lits)
                    {
                        continue;
                    }
                    if (l_idx > 0 && _step > tabulist[2 * var_idx + 1])
                    {
                        should_push_vec = true;
                        change_value = devide(-l->delta, l->pos_coff[i]);
                        // if(change_value>0) std::cout<<"error";
                    }
                    else if (l_idx < 0 && _step > tabulist[2 * var_idx])
                    {
                        should_push_vec = true;
                        change_value = devide(1 - l->delta, l->pos_coff[i]);
                    }
                    if (should_push_vec)
                    {
                        insert_operation(var_idx, change_value, operation_idx);
                    }
                    // if l_idx>0, delta should be <=0, while it is now >0(too large), so the var should enlarge by (-delta/coff) (this is a negative value), if l_idx<0, delta should be >=1, while it is now <1(too small), so the var should enlarge by (1-delta)/coff (positive value)
                }
                for (int i = 0; i < l->neg_coff.size(); i++)
                {
                    should_push_vec = false;
                    int var_idx = l->neg_coff_var_idx[i];
                    if (var_idx == lia_var_idx_with_most_lits)
                    {
                        continue;
                    }
                    if (l_idx > 0 && _step > tabulist[2 * var_idx])
                    {
                        should_push_vec = true;
                        change_value = devide(l->delta, l->neg_coff[i]);
                    }
                    else if (l_idx < 0 && _step > tabulist[2 * var_idx + 1])
                    {
                        should_push_vec = true;
                        change_value = devide(l->delta - 1, l->neg_coff[i]);
                    }
                    if (should_push_vec)
                    {
                        insert_operation(var_idx, change_value, operation_idx);
                    }
                    // if l_idx>0, delta should be <=0, while it is now >0(too large), so the var should enlarge by (delta/coff) (this is a positive value since the coff is neg), if l_idx<0, the delta should be >=1, while it is now <1(too small), so the var should enlarge by (delta-1)/coff (neg value)
                }
            }
        }
        // soft
        if (unsat_clauses->size() == 0)
        // if(issat==1)
        {
            for (int i = 0; i < unsat_soft_clauses->size(); i++)
            {
                soft_clause *cl = &(_soft_clauses[unsat_soft_clauses->element_at(i)]);
                for (int l_idx : cl->lia_literals)
                {
                    lit *l = &(_lits[std::abs(l_idx)]);
                    if (l->is_equal)
                    {
                        // to be addded
                    }
                    else
                    {
                        for (int i = 0; i < l->pos_coff.size(); i++)
                        {
                            should_push_vec = false;
                            int var_idx = l->pos_coff_var_idx[i];
                            if (var_idx == lia_var_idx_with_most_lits)
                            {
                                continue;
                            }
                            if (l_idx > 0 && _step > tabulist[2 * var_idx + 1])
                            {
                                should_push_vec = true;
                                change_value = devide(-l->delta, l->pos_coff[i]);
                            }
                            else if (l_idx < 0 && _step > tabulist[2 * var_idx])
                            {
                                should_push_vec = true;
                                change_value = devide(1 - l->delta, l->pos_coff[i]);
                            }
                            if (should_push_vec)
                            {
                                insert_operation(var_idx, change_value, operation_idx);
                            }
                            // if l_idx>0, delta should be <=0, while it is now >0(too large), so the var should enlarge by (-delta/coff) (this is a negative value), if l_idx<0, delta should be >=1, while it is now <1(too small), so the var should enlarge by (1-delta)/coff (positive value)
                        }
                        for (int i = 0; i < l->neg_coff.size(); i++)
                        {
                            should_push_vec = false;
                            int var_idx = l->neg_coff_var_idx[i];
                            if (var_idx == lia_var_idx_with_most_lits)
                            {
                                continue;
                            }
                            if (l_idx > 0 && _step > tabulist[2 * var_idx])
                            {
                                should_push_vec = true;
                                change_value = devide(l->delta, l->neg_coff[i]);
                            }
                            else if (l_idx < 0 && _step > tabulist[2 * var_idx + 1])
                            {
                                should_push_vec = true;
                                change_value = devide(l->delta - 1, l->neg_coff[i]);
                            }
                            if (should_push_vec)
                            {
                                insert_operation(var_idx, change_value, operation_idx);
                            }
                            // if l_idx>0, delta should be <=0, while it is now >0(too large), so the var should enlarge by (delta/coff) (this is a positive value since the coff is neg), if l_idx<0, the delta should be >=1, while it is now <1(too small), so the var should enlarge by (delta-1)/coff (neg value)
                        }
                    }
                }
            }
        }
        // soft
        //  go through the forward and backward move of vars, evaluate their score, pick the untabued best one
        if (operation_idx > 200)
        {
            BMS = true;
            cnt = 200;
            // BMS = false;
            // cnt = operation_idx;
        }
        else
        {
            BMS = false;
            cnt = operation_idx;
        }
        std::vector<int> goodvar;
        std::vector<__int128_t> goodv_change_value;
        std::vector<int> goodvar_score;
        std::vector<int> var_one_level; // hard>=0,soft>=0,score>0;
        std::vector<__int128_t> var_one_level_change_value;
        std::vector<int> var_one_level_score;
        std::vector<int> var_two_level; // hard>=0,soft<=0,score>0;
        std::vector<__int128_t> var_two_level_change_value;
        std::vector<int> var_two_level_score;
        std::vector<int> var_three_level; // hard<=0,soft>=0,score>0;
        std::vector<__int128_t> var_three_level_change_value;
        std::vector<int> var_three_level_score;
        for (int i = 0; i < cnt; i++)
        {
            if (BMS)
            {
                int idx = mt() % (operation_idx - i);
                operation_change_value = operation_change_value_vec[idx];
                operation_var_idx = operation_var_idx_vec[idx];
                operation_change_value_vec[idx] = operation_change_value_vec[operation_idx - i - 1];
                operation_var_idx_vec[idx] = operation_var_idx_vec[operation_idx - i - 1];
            }
            else
            {
                operation_change_value = operation_change_value_vec[i];
                operation_var_idx = operation_var_idx_vec[i];
            }
            two_level_score cur_score = critical_score_two_level(operation_var_idx, operation_change_value);
            if (cur_score.hard_score > 0 && cur_score.soft_score > 0)
            {
                var_one_level.push_back(operation_var_idx);
                var_one_level_change_value.push_back(operation_change_value);
                var_one_level_score.push_back(cur_score.score);
            }
            else if (cur_score.score > 0 && cur_score.soft_score > 0)
            {
                var_two_level.push_back(operation_var_idx);
                var_two_level_change_value.push_back(operation_change_value);
                var_two_level_score.push_back(cur_score.score);
            }
            else if (cur_score.score > 0 && cur_score.hard_score > 0)
            {
                // var_three_level.push_back(operation_var_idx);
                // var_three_level_change_value.push_back(operation_change_value);
                // var_three_level_score.push_back(cur_score.score);
            }
        }
        // std::cout<<"\n";
        if (goodvar.size() != 0 && mt() % 1000 > 999)
        {
            int index = rand() % goodvar.size();
            best_value = goodv_change_value[index];
            return goodvar[index];
        }
        int best_score_str = -1;
        int best_var_str = -1;
        int best_change_value_str = -1;
        uint64_t best_move_step_str = UINT64_MAX;
        if (var_one_level.size() != 0)
        {
            // std::cout<<"one"<<std::endl;
            for (int i = 0; i < var_one_level.size(); i++)
            {
                int opposite_direction_str = (var_one_level_change_value[i] > 0) ? 1 : 0; // if the change value is >0, then means it is moving forward, the opposite direction is 1(backward)
                uint64_t last_move_step_str = last_move[2 * var_one_level[i] + opposite_direction_str];
                if (var_one_level_score[i] > best_score_str)
                {
                    best_var_str = var_one_level[i];
                    best_change_value_str = var_one_level_change_value[i];
                    best_score_str = var_one_level_score[i];
                    // best_move_step_str=last_move_step_str;
                }
            }
        }
        else if (var_two_level.size() != 0)
        {
            // std::cout<<"two"<<std::endl;
            for (int i = 0; i < var_two_level.size(); i++)
            {
                int opposite_direction_str = (var_two_level_change_value[i] > 0) ? 1 : 0; // if the change value is >0, then means it is moving forward, the opposite direction is 1(backward)
                uint64_t last_move_step_str = last_move[2 * var_two_level[i] + opposite_direction_str];
                if (var_two_level_score[i] > best_score_str)
                {
                    best_var_str = var_two_level[i];
                    best_change_value_str = var_two_level_change_value[i];
                    best_score_str = var_two_level_score[i];
                    // best_move_step_str=last_move_step_str;
                }
            }
        }
        else if (var_three_level.size() != 0)
        {
            // std::cout<<"three"<<std::endl;
            for (int i = 0; i < var_three_level.size(); i++)
            {
                int opposite_direction_str = (var_three_level_change_value[i] > 0) ? 1 : 0; // if the change value is >0, then means it is moving forward, the opposite direction is 1(backward)
                uint64_t last_move_step_str = last_move[2 * var_three_level[i] + opposite_direction_str];
                if (var_three_level_score[i] > best_score_str)
                {
                    best_var_str = var_three_level[i];
                    best_change_value_str = var_three_level_change_value[i];
                    best_score_str = var_three_level_score[i];
                    // best_move_step_str=last_move_step_str;
                }
            }
        }
        else
        {
        }
        if (best_var_str != -1)
        {
            // std::cout<<best_var_idx<<"  "<<best_score<<std::endl;
            best_value = best_change_value_str;
            // std::cout<<best_var_str<<"  "<<print_128(best_value)<<std::endl;
            return best_var_str;
        }

        if (pick_critical_two_level() == 1)
        // if(0)
        {
            return -1;
        }
        else
        {
            if (mt() % 10000 > smooth_probability)
            {
                update_clause_weight();
            }
            else
            {
                smooth_clause_weight();
            }
            // std::cout<<"random walk"<<std::endl;
            random_walk();
            return -1;
        }
    }
    
    two_level_score ls_solver::critical_score_two_level(uint64_t var_idx, __int128_t change_value)
    {
        lit *l;
        clause *cp;
        soft_clause *scp;
        int critical_score = 0;
        __int128_t delta_old, delta_new, l_clause_idx;
        // number of make_lits in a clause
        int make_break_in_clause = 0;
        two_level_score cur;
        variable *var = &(_vars[var_idx]);
        for (int i = 0; i < var->literals.size(); i++)
        {
            l = &(_lits[std::abs(var->literals[i])]);
            l_clause_idx = var->literal_clause[i];
            delta_old = l->delta;
            delta_new = delta_old + (var->literal_coff[i] * change_value); // l_clause_idx means that the coff is positive, and vice versa
            if ((!l->is_equal && delta_old <= 0 && delta_new > 0) || (l->is_equal && delta_old == 0 && delta_new != 0))
                make_break_in_clause = (var->literals[i] > 0) ? (make_break_in_clause - 1) : (make_break_in_clause + 1);
            else if ((!l->is_equal && delta_old > 0 && delta_new <= 0) || (l->is_equal && delta_old != 0 && delta_new == 0))
                make_break_in_clause = (var->literals[i] > 0) ? (make_break_in_clause + 1) : (make_break_in_clause - 1);
            // enter a new clause or the last literal
            if ((i != (var->literals.size() - 1) && l_clause_idx != var->literal_clause[i + 1]) || i == (var->literals.size() - 1))
            {
                cp = &(_clauses[abs_128(l_clause_idx)]);
                if (cp->sat_count == 0 && cp->sat_count + make_break_in_clause > 0)
                    critical_score += cp->weight;
                else if (cp->sat_count > 0 && cp->sat_count + make_break_in_clause == 0)
                    critical_score -= cp->weight;
                make_break_in_clause = 0;
            }
        }
        cur.hard_score = critical_score;
        // soft
        make_break_in_clause = 0;
        for (int i = 0; i < var->soft_literals.size(); i++)
        {
            l = &(_lits[std::abs(var->soft_literals[i])]);
            l_clause_idx = var->soft_literal_clause[i];
            delta_old = l->delta;
            delta_new = delta_old + (var->soft_literal_coff[i] * change_value); // l_clause_idx means that the coff is positive, and vice versa
            if ((!l->is_equal && delta_old <= 0 && delta_new > 0) || (l->is_equal && delta_old == 0 && delta_new != 0))
                make_break_in_clause = (var->soft_literals[i] > 0) ? (make_break_in_clause - 1) : (make_break_in_clause + 1);
            else if ((!l->is_equal && delta_old > 0 && delta_new <= 0) || (l->is_equal && delta_old != 0 && delta_new == 0))
                make_break_in_clause = (var->soft_literals[i] > 0) ? (make_break_in_clause + 1) : (make_break_in_clause - 1);
            // enter a new clause or the last literal
            if ((i != (var->soft_literals.size() - 1) && l_clause_idx != var->soft_literal_clause[i + 1]) || i == (var->soft_literals.size() - 1))
            {
                scp = &(_soft_clauses[abs_128(l_clause_idx)]);
                if (scp->sat_count == 0 && scp->sat_count + make_break_in_clause > 0)
                    critical_score += scp->weight;
                else if (scp->sat_count > 0 && scp->sat_count + make_break_in_clause == 0)
                    critical_score -= scp->weight;
                make_break_in_clause = 0;
            }
        }
        cur.soft_score = critical_score - cur.hard_score;
        cur.score = critical_score;
        return cur;
        // soft
        // return critical_score;
    }
    
    two_level_triple ls_solver::two_level_find_best_tabu()
    {
        int best_score, score, operation_var_idx, best_var_idx, cnt;
        __int128_t operation_change_value, change_value, best_value;
        bool BMS = false;
        bool should_push_vec;
        best_score = (is_idl) ? 0 : 1;
        // best_score = INT16_MIN;
        best_var_idx = -1;
        change_value = 0;
        uint64_t best_last_move = UINT64_MAX;
        int operation_idx = 0;
        // determine the critical value
        for (int i = 0; i < unsat_clauses->size(); i++)
        {
            clause *cl = &(_clauses[unsat_clauses->element_at(i)]);
            for (int l_idx : cl->lia_literals)
            {
                lit *l = &(_lits[std::abs(l_idx)]);
                if (l->is_equal)
                {
                    if (l_idx < 0)
                    {
                        for (int var_idx : l->pos_coff_var_idx)
                        {
                            if (_step > tabulist[2 * var_idx])
                            {
                                insert_operation(var_idx, 1, operation_idx);
                            }
                            if (_step > tabulist[2 * var_idx + 1])
                            {
                                insert_operation(var_idx, -1, operation_idx);
                            }
                        }
                        for (int var_idx : l->neg_coff_var_idx)
                        {
                            if (_step > tabulist[2 * var_idx])
                            {
                                insert_operation(var_idx, 1, operation_idx);
                            }
                            if (_step > tabulist[2 * var_idx + 1])
                            {
                                insert_operation(var_idx, -1, operation_idx);
                            }
                        }
                    } // delta should not be 0, while it is 0, so the var should increase 1/-1
                    else
                    {
                        for (int j = 0; j < l->pos_coff.size(); j++)
                        {
                            int var_idx = l->pos_coff_var_idx[j];
                            if ((l->delta % l->pos_coff[j]) != 0)
                            {
                                continue;
                            }
                            if ((l->delta < 0 && _step > tabulist[2 * var_idx]) || (l->delta > 0 && _step > tabulist[2 * var_idx + 1]))
                            {
                                insert_operation(var_idx, (-l->delta / l->pos_coff[j]), operation_idx);
                            }
                        }
                        for (int j = 0; j < l->neg_coff.size(); j++)
                        {
                            int var_idx = l->neg_coff_var_idx[j];
                            if ((l->delta % l->neg_coff[j]) != 0)
                            {
                                continue;
                            }
                            if ((l->delta > 0 && _step > tabulist[2 * var_idx]) || (l->delta < 0 && _step > tabulist[2 * var_idx + 1]))
                            {
                                insert_operation(var_idx, (l->delta / l->neg_coff[j]), operation_idx);
                            }
                        }
                    } // delta should be 0, while it is not 0, so the var should increase (-delta/coff), while (-delta%coff)==0
                    continue;
                }
                for (int i = 0; i < l->pos_coff.size(); i++)
                {
                    should_push_vec = false;
                    int var_idx = l->pos_coff_var_idx[i];
                    if (var_idx == lia_var_idx_with_most_lits)
                    {
                        continue;
                    }
                    if (l_idx > 0 && _step > tabulist[2 * var_idx + 1])
                    // if (l_idx > 0)
                    {
                        should_push_vec = true;
                        change_value = devide(-l->delta, l->pos_coff[i]);
                    }
                    else if (l_idx < 0 && _step > tabulist[2 * var_idx])
                    // else if (l_idx < 0)
                    {
                        should_push_vec = true;
                        change_value = devide(1 - l->delta, l->pos_coff[i]);
                    }
                    if (should_push_vec)
                    {
                        insert_operation(var_idx, change_value, operation_idx);
                    }
                    // if l_idx>0, delta should be <=0, while it is now >0(too large), so the var should enlarge by (-delta/coff) (this is a negative value), if l_idx<0, delta should be >=1, while it is now <1(too small), so the var should enlarge by (1-delta)/coff (positive value)
                }
                for (int i = 0; i < l->neg_coff.size(); i++)
                {
                    should_push_vec = false;
                    int var_idx = l->neg_coff_var_idx[i];
                    if (var_idx == lia_var_idx_with_most_lits)
                    {
                        continue;
                    }
                    if (l_idx > 0 && _step > tabulist[2 * var_idx])
                    // if (l_idx > 0 )
                    {
                        should_push_vec = true;
                        change_value = devide(l->delta, l->neg_coff[i]);
                    }
                    else if (l_idx < 0 && _step > tabulist[2 * var_idx + 1])
                    // else if (l_idx < 0 )
                    {
                        should_push_vec = true;
                        change_value = devide(l->delta - 1, l->neg_coff[i]);
                    }
                    if (should_push_vec)
                    {
                        insert_operation(var_idx, change_value, operation_idx);
                    }
                    // if l_idx>0, delta should be <=0, while it is now >0(too large), so the var should enlarge by (delta/coff) (this is a positive value since the coff is neg), if l_idx<0, the delta should be >=1, while it is now <1(too small), so the var should enlarge by (delta-1)/coff (neg value)
                }
            }
        }
        // soft
        for (int i = 0; i < unsat_soft_clauses->size(); i++)
        {
            soft_clause *cl = &(_soft_clauses[unsat_soft_clauses->element_at(i)]);
            for (int l_idx : cl->lia_literals)
            {
                lit *l = &(_lits[std::abs(l_idx)]);
                if (l->is_equal)
                {
                    // to be addded
                }
                else
                {
                    for (int i = 0; i < l->pos_coff.size(); i++)
                    {
                        should_push_vec = false;
                        int var_idx = l->pos_coff_var_idx[i];
                        if (var_idx == lia_var_idx_with_most_lits)
                        {
                            continue;
                        }
                        if (l_idx > 0 && _step > tabulist[2 * var_idx + 1])
                        // if (l_idx > 0 )
                        {
                            should_push_vec = true;
                            change_value = devide(-l->delta, l->pos_coff[i]);
                        }
                        else if (l_idx < 0 && _step > tabulist[2 * var_idx])
                        // else if (l_idx )
                        {
                            should_push_vec = true;
                            change_value = devide(1 - l->delta, l->pos_coff[i]);
                        }
                        if (should_push_vec)
                        {
                            insert_operation(var_idx, change_value, operation_idx);
                        }
                        // if l_idx>0, delta should be <=0, while it is now >0(too large), so the var should enlarge by (-delta/coff) (this is a negative value), if l_idx<0, delta should be >=1, while it is now <1(too small), so the var should enlarge by (1-delta)/coff (positive value)
                    }
                    for (int i = 0; i < l->neg_coff.size(); i++)
                    {
                        should_push_vec = false;
                        int var_idx = l->neg_coff_var_idx[i];
                        if (var_idx == lia_var_idx_with_most_lits)
                        {
                            continue;
                        }
                        if (l_idx > 0 && _step > tabulist[2 * var_idx])
                        // if (l_idx > 0 )
                        {
                            should_push_vec = true;
                            change_value = devide(l->delta, l->neg_coff[i]);
                        }
                        else if (l_idx < 0 && _step > tabulist[2 * var_idx + 1])
                        // else if (l_idx < 0 )
                        {
                            should_push_vec = true;
                            change_value = devide(l->delta - 1, l->neg_coff[i]);
                        }
                        if (should_push_vec)
                        {
                            insert_operation(var_idx, change_value, operation_idx);
                        }
                        // if l_idx>0, delta should be <=0, while it is now >0(too large), so the var should enlarge by (delta/coff) (this is a positive value since the coff is neg), if l_idx<0, the delta should be >=1, while it is now <1(too small), so the var should enlarge by (delta-1)/coff (neg value)
                    }
                }
            }
        }
        // soft
        //  go through the forward and backward move of vars, evaluate their score, pick the untabued best one
        if (operation_idx > 200)
        {
            BMS = true;
            cnt = 200;
            // BMS = false;
            // cnt = operation_idx;
        }
        else
        {
            BMS = false;
            cnt = operation_idx;
        }
        std::vector<int> goodvar;
        std::vector<__int128_t> goodv_change_value;
        for (int i = 0; i < cnt; i++)
        {
            if (BMS)
            {
                int idx = mt() % (operation_idx - i);
                operation_change_value = operation_change_value_vec[idx];
                operation_var_idx = operation_var_idx_vec[idx];
                operation_change_value_vec[idx] = operation_change_value_vec[operation_idx - i - 1];
                operation_var_idx_vec[idx] = operation_var_idx_vec[operation_idx - i - 1];
            }
            else
            {
                operation_change_value = operation_change_value_vec[i];
                operation_var_idx = operation_var_idx_vec[i];
            }
            score = critical_score(operation_var_idx, operation_change_value);
            if (score > 0)
            {
                goodvar.push_back(operation_var_idx);
                goodv_change_value.push_back(operation_change_value);
            }
            int opposite_direction = (operation_change_value > 0) ? 1 : 0; // if the change value is >0, then means it is moving forward, the opposite direction is 1(backward)
            uint64_t last_move_step = last_move[2 * operation_var_idx + opposite_direction];
            if (score > best_score || (score == best_score /*&&score>0*/ && last_move_step < best_last_move))
            {
                best_score = score;
                best_var_idx = operation_var_idx;
                best_value = operation_change_value;
                best_last_move = last_move_step;
            }
        }
        two_level_triple best_two_level;
        if (best_var_idx != -1)
        {
            best_two_level.score = best_score;
            best_two_level.var_idx = best_var_idx;
            best_two_level.change_value = best_value;
        }
        else
        {
            best_two_level.score = -1;
            best_two_level.var_idx = -1;
            best_two_level.change_value = -1;
        }
        return best_two_level;
    }
    
    int ls_solver::pick_bool_lia_var()
    {
        for (int i = 0; i < bool_var_vec.size(); i++)
        {
            critical_move(bool_var_vec[i], 0);
            two_level_triple cur = two_level_find_best();
            if (cur.var_idx != -1 && cur.score > 0)
            {
                critical_move(cur.var_idx, cur.change_value);
                update_clause_weight();
                return 1;
            }
            critical_move(bool_var_vec[i], 0);
        }
        return -1;
    }
    
    void ls_solver::critical_move_pse(uint64_t var_idx, __int128_t change_value)
    {
        // std::cout<<var_idx<<" " <<print_128(change_value)<<std::endl;//print -1 max not answer
        int direction = (change_value > 0) ? 0 : 1;
        if (_vars[var_idx].is_lia)
        {
            critical_score_subscore(var_idx, change_value);
            _solution[var_idx] += change_value;
        }
        else
        {
            // std::cout<<"here"<<std::endl;
            int origin_score = _vars[var_idx].score;
            critical_score_subscore(var_idx);
            _solution[var_idx] *= -1;
            _vars[var_idx].score = -origin_score;
        }
        // step
        if (_vars[var_idx].is_lia)
        {
            // last_move[2 * var_idx + direction] = _step;
            // tabulist[var_idx * 2 + (direction + 1) % 2] = _step + 2;
            // if(is_weighted==1) tabulist[var_idx * 2 + (direction + 1) % 2] = _step+1;
            // else tabulist[var_idx * 2 + (direction + 1) % 2] = _step + 3 + mt() % 10;
            // if (CC_mode != -1)
            // {
            //     modify_CC(var_idx, direction);
            //}
        }
        else
        {
            // last_move[2 * var_idx] = _outer_layer_step;
            // tabulist[2 * var_idx] = _outer_layer_step + 1 + mt() % 3;
            // if (CC_mode != -1)
            // {
            //     modify_CC(var_idx, direction);
            // }
            // _outer_layer_step++;
        }
    }
    
    void ls_solver::restart()
    {
        clear_prev_data();
        construct_slution_score();
        initialize_lit_datas();
        initialize_clause_datas();
        best_found_this_restart = unsat_clauses->size();
        int best_obj = 0;
        for (int i = 0; i < _soft_clauses.size(); i++)
        {
            best_obj += _soft_clauses[i].org_weight;
        }
        if (best_obj < best_found_obj)
            best_found_obj = best_obj;
        update_best_solution();
    }
    
    int ls_solver::critical_score_pair(uint64_t var_idx_1, __int128_t change_value_1, uint64_t var_idx_2, __int128_t change_value_2)
    {
        lit *l;
        clause *cp;
        soft_clause *scp;
        int critical_score = 0;
        __int128_t delta_old, delta_new, l_clause_idx;
        // number of make_lits in a clause
        int make_break_in_clause = 0;
        variable *var_1 = &(_vars[var_idx_1]);
        variable *var_2 = &(_vars[var_idx_2]);
        std::vector<int> mid_delta_lit_idx;
        std::vector<int> mid_sat_count_vec;
        for (int i = 0; i < var_1->literals.size(); i++)
        {
            l = &(_lits[std::abs(var_1->literals[i])]);
            l_clause_idx = var_1->literal_clause[i];
            delta_old = l->delta;
            delta_new = delta_old + (var_1->literal_coff[i] * change_value_1); // l_clause_idx means that the coff is positive, and vice versa
            l->mid_delta = delta_new;
            mid_delta_lit_idx.push_back(std::abs(var_1->literals[i]));
            if ((!l->is_equal && delta_old <= 0 && delta_new > 0) || (l->is_equal && delta_old == 0 && delta_new != 0))
                make_break_in_clause = (var_1->literals[i] > 0) ? (make_break_in_clause - 1) : (make_break_in_clause + 1);
            else if ((!l->is_equal && delta_old > 0 && delta_new <= 0) || (l->is_equal && delta_old != 0 && delta_new == 0))
                make_break_in_clause = (var_1->literals[i] > 0) ? (make_break_in_clause + 1) : (make_break_in_clause - 1);
            // enter a new clause or the last literal
            if ((i != (var_1->literals.size() - 1) && l_clause_idx != var_1->literal_clause[i + 1]) || i == (var_1->literals.size() - 1))
            {
                cp = &(_clauses[abs_128(l_clause_idx)]);
                if (cp->sat_count == 0 && cp->sat_count + make_break_in_clause > 0)
                    critical_score += cp->weight;
                else if (cp->sat_count > 0 && cp->sat_count + make_break_in_clause == 0)
                    critical_score -= cp->weight;
                cp->mid_sat_count = cp->sat_count + make_break_in_clause; // new
                mid_sat_count_vec.push_back(abs_128(l_clause_idx));       // new
                make_break_in_clause = 0;
            }
        }
        for (int i = 0; i < var_2->literals.size(); i++)
        {
            l = &(_lits[std::abs(var_2->literals[i])]);
            l_clause_idx = var_2->literal_clause[i];
            if (l->mid_delta == INT32_MIN)
            {
                delta_old = l->delta;
                delta_new = delta_old + (var_2->literal_coff[i] * change_value_2); // l_clause_idx means that the coff is positive, and vice versa
            }
            else
            {
                delta_old = l->mid_delta;
                delta_new = delta_old + (var_2->literal_coff[i] * change_value_2);
            }
            if ((!l->is_equal && delta_old <= 0 && delta_new > 0) || (l->is_equal && delta_old == 0 && delta_new != 0))
                make_break_in_clause = (var_2->literals[i] > 0) ? (make_break_in_clause - 1) : (make_break_in_clause + 1);
            else if ((!l->is_equal && delta_old > 0 && delta_new <= 0) || (l->is_equal && delta_old != 0 && delta_new == 0))
                make_break_in_clause = (var_2->literals[i] > 0) ? (make_break_in_clause + 1) : (make_break_in_clause - 1);
            // enter a new clause or the last literal
            if ((i != (var_2->literals.size() - 1) && l_clause_idx != var_2->literal_clause[i + 1]) || i == (var_2->literals.size() - 1))
            {
                cp = &(_clauses[abs_128(l_clause_idx)]);
                if (cp->mid_sat_count == -1)
                {
                    if (cp->sat_count == 0 && cp->sat_count + make_break_in_clause > 0)
                        critical_score += cp->weight;
                    else if (cp->sat_count > 0 && cp->sat_count + make_break_in_clause == 0)
                        critical_score -= cp->weight;
                }
                else
                {
                    if (cp->mid_sat_count == 0 && cp->mid_sat_count + make_break_in_clause > 0)
                        critical_score += cp->weight;
                    else if (cp->mid_sat_count > 0 && cp->mid_sat_count + make_break_in_clause == 0)
                        critical_score -= cp->weight;
                }
                make_break_in_clause = 0;
            }
        }
        for (int i = 0; i < mid_delta_lit_idx.size(); i++)
        {
            l = &(_lits[mid_delta_lit_idx[i]]);
            l->mid_delta = INT32_MIN;
        }
        for (int i = 0; i < mid_sat_count_vec.size(); i++)
        {
            cp = &(_clauses[mid_sat_count_vec[i]]);
            cp->mid_sat_count = -1;
        }
        // soft
        mid_sat_count_vec.clear();
        mid_delta_lit_idx.clear();
        make_break_in_clause = 0;
        for (int i = 0; i < var_1->soft_literals.size(); i++)
        {
            l = &(_lits[std::abs(var_1->soft_literals[i])]);
            l_clause_idx = var_1->soft_literal_clause[i];
            delta_old = l->delta;
            delta_new = delta_old + (var_1->soft_literal_coff[i] * change_value_1); // l_clause_idx means that the coff is positive, and vice versa
            // if(l->mid_delta==0) l->mid_delta=delta_new;
            // else l->mid_delta+=delta_new;
            l->mid_delta = delta_new;
            mid_delta_lit_idx.push_back(std::abs(var_1->soft_literals[i]));
            if ((!l->is_equal && delta_old <= 0 && delta_new > 0) || (l->is_equal && delta_old == 0 && delta_new != 0))
                make_break_in_clause = (var_1->soft_literals[i] > 0) ? (make_break_in_clause - 1) : (make_break_in_clause + 1);
            else if ((!l->is_equal && delta_old > 0 && delta_new <= 0) || (l->is_equal && delta_old != 0 && delta_new == 0))
                make_break_in_clause = (var_1->soft_literals[i] > 0) ? (make_break_in_clause + 1) : (make_break_in_clause - 1);
            // enter a new clause or the last literal
            if ((i != (var_1->soft_literals.size() - 1) && l_clause_idx != var_1->soft_literal_clause[i + 1]) || i == (var_1->soft_literals.size() - 1))
            {
                scp = &(_soft_clauses[abs_128(l_clause_idx)]);
                if (scp->sat_count == 0 && scp->sat_count + make_break_in_clause > 0)
                    critical_score += scp->weight;
                else if (scp->sat_count > 0 && scp->sat_count + make_break_in_clause == 0)
                    critical_score -= scp->weight;
                scp->mid_sat_count = scp->sat_count + make_break_in_clause; // new
                mid_sat_count_vec.push_back(abs_128(l_clause_idx));         // new
                make_break_in_clause = 0;
            }
        }
        for (int i = 0; i < var_2->soft_literals.size(); i++)
        {
            l = &(_lits[std::abs(var_2->soft_literals[i])]);
            l_clause_idx = var_2->soft_literal_clause[i];
            if (l->mid_delta == INT32_MIN)
            {
                delta_old = l->delta;
                delta_new = delta_old + (var_2->soft_literal_coff[i] * change_value_2); // l_clause_idx means that the coff is positive, and vice versa
            }
            else
            {
                delta_old = l->mid_delta;
                delta_new = delta_old + (var_2->soft_literal_coff[i] * change_value_2);
            }
            if ((!l->is_equal && delta_old <= 0 && delta_new > 0) || (l->is_equal && delta_old == 0 && delta_new != 0))
                make_break_in_clause = (var_2->soft_literals[i] > 0) ? (make_break_in_clause - 1) : (make_break_in_clause + 1);
            else if ((!l->is_equal && delta_old > 0 && delta_new <= 0) || (l->is_equal && delta_old != 0 && delta_new == 0))
                make_break_in_clause = (var_2->soft_literals[i] > 0) ? (make_break_in_clause + 1) : (make_break_in_clause - 1);
            // enter a new clause or the last literal
            if ((i != (var_2->soft_literals.size() - 1) && l_clause_idx != var_2->soft_literal_clause[i + 1]) || i == (var_2->soft_literals.size() - 1))
            {
                scp = &(_soft_clauses[abs_128(l_clause_idx)]);
                if (scp->mid_sat_count == -1)
                {
                    if (scp->sat_count == 0 && scp->sat_count + make_break_in_clause > 0)
                        critical_score += scp->weight;
                    else if (scp->sat_count > 0 && scp->sat_count + make_break_in_clause == 0)
                        critical_score -= scp->weight;
                }
                else
                {
                    if (scp->mid_sat_count == 0 && scp->mid_sat_count + make_break_in_clause > 0)
                        critical_score += scp->weight;
                    else if (scp->mid_sat_count > 0 && scp->mid_sat_count + make_break_in_clause == 0)
                        critical_score -= scp->weight;
                }
                make_break_in_clause = 0;
            }
        }
        for (int i = 0; i < mid_delta_lit_idx.size(); i++)
        {
            l = &(_lits[mid_delta_lit_idx[i]]);
            l->mid_delta = INT32_MIN;
        }
        for (int i = 0; i < mid_sat_count_vec.size(); i++)
        {
            scp = &(_soft_clauses[mid_sat_count_vec[i]]);
            scp->mid_sat_count = -1;
        }
        // soft
        return critical_score;
        //     lit *l;
        //     clause *cp;
        //     soft_clause *scp;
        //     int critical_score = 0;
        //     __int128_t delta_old, delta_new, l_clause_idx;
        //     // number of make_lits in a clause
        //     int make_break_in_clause = 0;
        //     variable *var_1 = &(_vars[var_idx_1]);
        //     variable *var_2 = &(_vars[var_idx_2]);
        //     std::unordered_map<int, std::vector<lit_info>> clause_lit_map;
        //     for (int i = 0; i < var_1->literals.size(); i++)
        //     {
        //         l_clause_idx = var_1->literal_clause[i];
        //         if(clause_lit_map.find(l_clause_idx)==clause_lit_map.end())
        //         {
        //             lit_info cur;
        //             std::vector<lit_info>lit_info_vec;
        //             cur.lit_idx=abs(var_1->literals[i]);cur.lit_delt=(var_1->literal_coff[i] * change_value_1);
        //             if(var_1->literals[i] > 0) cur.is_true=1;
        //             else cur.is_true=0;
        //             lit_info_vec.push_back(cur);
        //             clause_lit_map.insert(make_pair(l_clause_idx,lit_info_vec));
        //         }
        //         else /*if(clause_lit_map.find(l_clause_idx)!=clause_lit_map.end())*/
        //         {
        //             lit_info cur;
        //             cur.lit_idx=abs(var_1->literals[i]);cur.lit_delt=(var_1->literal_coff[i] * change_value_1);
        //             if(var_1->literals[i] > 0) cur.is_true=1;
        //             else cur.is_true=0;
        //             clause_lit_map.find(l_clause_idx)->second.push_back(cur);
        //         }
        //     }
        //     for(int i=0;i<var_2->literals.size();i++)
        //     {
        //         l_clause_idx = var_2->literal_clause[i];
        //         //if(l_clause_idx==102&&bug_flag==1) std::cout<<"here0"<<std::endl;
        //         if(clause_lit_map.find(l_clause_idx)==clause_lit_map.end())
        //         {
        //             lit_info cur;
        //             std::vector<lit_info>lit_info_vec;
        //             cur.lit_idx=abs(var_2->literals[i]);cur.lit_delt=(var_2->literal_coff[i] * change_value_2);
        //             if(var_2->literals[i] > 0) cur.is_true=1;
        //             else cur.is_true=0;
        //             lit_info_vec.push_back(cur);
        //             clause_lit_map.insert(make_pair(l_clause_idx,lit_info_vec));
        //         }
        //         else /*if(clause_lit_map.find(l_clause_idx)!=clause_lit_map.end())*/
        //         {
        //             lit_info cur;
        //             cur.lit_idx=abs(var_2->literals[i]);cur.lit_delt=(var_2->literal_coff[i] * change_value_2);
        //             if(var_2->literals[i] > 0) cur.is_true=1;
        //             else cur.is_true=0;
        //             std::vector<lit_info> has_vec=clause_lit_map.find(l_clause_idx)->second;
        //             int index=find(has_vec.begin(),has_vec.end(),cur)-has_vec.begin();
        //             if(index==has_vec.end()-has_vec.begin())
        //             {
        //                 clause_lit_map.find(l_clause_idx)->second.push_back(cur);
        //             }
        //             else
        //             {
        //                 //int index=find(has_vec.begin(),has_vec.end(),cur)-has_vec.begin();
        //                 //if(cur.lit_idx==109) std::cout<<" !!!!! "<<print_128(clause_lit_map.find(l_clause_idx)->second[index].lit_delt)<<"     "<<print_128(cur.lit_delt)<<std::endl;
        //                 clause_lit_map.find(l_clause_idx)->second[index].lit_delt+=cur.lit_delt;
        //             }
        //         }
        //     }
        //     std::unordered_map<int, std::vector<lit_info>>::iterator iter;
        //     //if(bug_flag==1) std::cout<<print_128(change_value_1)<<"  "<<print_128(change_value_2)<<std::endl;
        //     for(iter=clause_lit_map.begin();iter!=clause_lit_map.end();iter++)
        //     {
        //         int clause_idx=iter->first;
        //        // if(bug_flag==1) std::cout<<clause_idx<<":  "<<std::endl;
        //         cp = &(_clauses[abs_128(clause_idx)]);
        //         std::vector<lit_info>lit_info_vec=iter->second;
        //         make_break_in_clause=0;
        //         for(int i=0;i<lit_info_vec.size();i++)
        //         {
        //             lit_info cur=lit_info_vec[i];
        //             l = &(_lits[std::abs(cur.lit_idx)]);
        //             delta_old = l->delta;
        //             delta_new=delta_old+cur.lit_delt;
        //             //if(bug_flag==1) std::cout<<cur.lit_idx<<"  "<<print_128(cur.lit_delt)<<std::endl;
        //             if ((!l->is_equal && delta_old <= 0 && delta_new > 0) || (l->is_equal && delta_old == 0 && delta_new != 0))
        //                 make_break_in_clause = (cur.is_true > 0) ? (make_break_in_clause - 1) : (make_break_in_clause + 1);
        //             else if ((!l->is_equal && delta_old > 0 && delta_new <= 0) || (l->is_equal && delta_old != 0 && delta_new == 0))
        //                 make_break_in_clause = (cur.is_true > 0) ? (make_break_in_clause + 1) : (make_break_in_clause - 1);
        //         }
        //         if (cp->sat_count == 0 && cp->sat_count + make_break_in_clause > 0)
        //             critical_score += cp->weight;
        //         else if (cp->sat_count > 0 && cp->sat_count + make_break_in_clause == 0)
        //             critical_score -= cp->weight;
        //     }
        //     // //soft
        //     // make_break_in_clause = 0;
        //     // for (int i = 0; i < var->soft_literals.size(); i++)
        //     // {
        //     //     l = &(_lits[std::abs(var->soft_literals[i])]);
        //     //     l_clause_idx = var->soft_literal_clause[i];
        //     //     delta_old = l->delta;
        //     //     delta_new = delta_old + (var->soft_literal_coff[i] * change_value); // l_clause_idx means that the coff is positive, and vice versa
        //     //     if ((!l->is_equal && delta_old <= 0 && delta_new > 0) || (l->is_equal && delta_old == 0 && delta_new != 0))
        //     //         make_break_in_clause = (var->soft_literals[i] > 0) ? (make_break_in_clause - 1) : (make_break_in_clause + 1);
        //     //     else if ((!l->is_equal && delta_old > 0 && delta_new <= 0) || (l->is_equal && delta_old != 0 && delta_new == 0))
        //     //         make_break_in_clause = (var->soft_literals[i] > 0) ? (make_break_in_clause + 1) : (make_break_in_clause - 1);
        //     //     // enter a new clause or the last literal
        //     //     if ((i != (var->soft_literals.size() - 1) && l_clause_idx != var->soft_literal_clause[i + 1]) || i == (var->soft_literals.size() - 1))
        //     //     {
        //     //         scp = &(_soft_clauses[abs_128(l_clause_idx)]);
        //     //         if (scp->sat_count == 0 && scp->sat_count + make_break_in_clause > 0)
        //     //             critical_score += scp->weight;
        //     //         else if (scp->sat_count > 0 && scp->sat_count + make_break_in_clause == 0)
        //     //             critical_score -= scp->weight;
        //     //         make_break_in_clause = 0;
        //     //     }
        //     // }
        //     //to be test & think
        //     std::unordered_map<int, std::vector<lit_info>> soft_clause_lit_map;
        //     for (int i = 0; i < var_1->soft_literals.size(); i++)
        //     {
        //         l_clause_idx = var_1->soft_literal_clause[i];
        //         // if(soft_clause_lit_map.find(l_clause_idx)==soft_clause_lit_map.end())
        //         // {
        //             lit_info cur;
        //             std::vector<lit_info>lit_info_vec;
        //             cur.lit_idx=abs(var_1->soft_literals[i]);cur.lit_delt=(var_1->soft_literal_coff[i] * change_value_1);
        //             if(var_1->soft_literals[i] > 0) cur.is_true=1;
        //             else cur.is_true=0;
        //             lit_info_vec.push_back(cur);
        //             soft_clause_lit_map.insert(make_pair(l_clause_idx,lit_info_vec));
        //         //}
        //         // else if(soft_clause_lit_map.find(l_clause_idx)!=soft_clause_lit_map.end())
        //         // {
        //         //     lit_info cur;
        //         //     cur.lit_idx=abs(var_1->soft_literals[i]);cur.lit_delt=(var_1->soft_literal_coff[i] * change_value_1);
        //         //     if(var_1->soft_literals[i] > 0) cur.is_true=1;
        //         //     else cur.is_true=0;
        //         //     soft_clause_lit_map.find(l_clause_idx)->second.push_back(cur);
        //         // }
        //     }
        //     for(int i=0;i<var_2->soft_literals.size();i++)
        //     {
        //         l_clause_idx = var_2->soft_literal_clause[i];
        //         if(soft_clause_lit_map.find(l_clause_idx)==soft_clause_lit_map.end())
        //         {
        //             lit_info cur;
        //             std::vector<lit_info>lit_info_vec;
        //             cur.lit_idx=abs(var_2->soft_literals[i]);cur.lit_delt=(var_2->soft_literal_coff[i] * change_value_2);
        //             if(var_2->soft_literals[i] > 0) cur.is_true=1;
        //             else cur.is_true=0;
        //             lit_info_vec.push_back(cur);
        //             soft_clause_lit_map.insert(make_pair(l_clause_idx,lit_info_vec));
        //         }
        //         else /*if(soft_clause_lit_map.find(l_clause_idx)!=soft_clause_lit_map.end())*/
        //         {
        //             lit_info cur;
        //             cur.lit_idx=abs(var_2->soft_literals[i]);cur.lit_delt=(var_2->soft_literal_coff[i] * change_value_2);
        //             if(var_2->soft_literals[i] > 0) cur.is_true=1;
        //             else cur.is_true=0;
        //             std::vector<lit_info> has_vec=soft_clause_lit_map.find(l_clause_idx)->second;
        //             // if(find(has_vec.begin(),has_vec.end(),cur)==has_vec.end())
        //             // {
        //             //     soft_clause_lit_map.find(l_clause_idx)->second.push_back(cur);
        //             // }
        //             // else
        //             // {
        //                 int index=find(has_vec.begin(),has_vec.end(),cur)-has_vec.begin();
        //                 soft_clause_lit_map.find(l_clause_idx)->second[index].lit_delt+=cur.lit_delt;
        //             //}
        //         }
        //     }
        //     //std::map<int, std::vector<lit_info>>::iterator iter;
        //     for(iter=soft_clause_lit_map.begin();iter!=soft_clause_lit_map.end();iter++)
        //     {
        //         int clause_idx=iter->first;
        //         scp = &(_soft_clauses[abs_128(clause_idx)]);
        //         std::vector<lit_info>lit_info_vec=iter->second;
        //         make_break_in_clause=0;
        //         for(int i=0;i<lit_info_vec.size();i++)
        //         {
        //             lit_info cur=lit_info_vec[i];
        //             l = &(_lits[std::abs(cur.lit_idx)]);
        //             delta_old = l->delta;
        //             delta_new=delta_old+cur.lit_delt;
        //             if ((!l->is_equal && delta_old <= 0 && delta_new > 0) || (l->is_equal && delta_old == 0 && delta_new != 0))
        //                 make_break_in_clause = (cur.is_true > 0) ? (make_break_in_clause - 1) : (make_break_in_clause + 1);
        //             else if ((!l->is_equal && delta_old > 0 && delta_new <= 0) || (l->is_equal && delta_old != 0 && delta_new == 0))
        //                 make_break_in_clause = (cur.is_true > 0) ? (make_break_in_clause + 1) : (make_break_in_clause - 1);
        //         }
        //         if (scp->sat_count == 0 && scp->sat_count + make_break_in_clause > 0)
        //             critical_score += scp->weight;
        //         else if (scp->sat_count > 0 && scp->sat_count + make_break_in_clause == 0)
        //             critical_score -= scp->weight;
        //     }
        //     //soft
        //     return critical_score;
    }
    
    void ls_solver::insert_operation_pair(int var_idx, __int128_t change_value, int &operation_idx, int l_idx)
    {
        __int128_t future_solution = _solution[var_idx] + change_value;
        bool no_pre_value = (_pre_value_1[var_idx] == INT32_MAX && _pre_value_2[var_idx] == INT32_MAX && future_solution >= _vars[var_idx].low_bound && future_solution <= _vars[var_idx].upper_bound);
        bool has_pre_value_1 = (_pre_value_1[var_idx] != INT32_MAX && _pre_value_2[var_idx] == INT32_MAX && future_solution == _pre_value_1[var_idx]);
        bool has_pre_value_2 = (_pre_value_1[var_idx] != INT32_MAX && _pre_value_2[var_idx] != INT32_MAX && (future_solution == _pre_value_1[var_idx] || future_solution == _pre_value_2[var_idx]));
        if (no_pre_value || has_pre_value_1 || has_pre_value_2)
        // if (1)
        {
            //    if(future_solution>=_vars[var_idx].low_bound&&future_solution<=_vars[var_idx].upper_bound){
            operation_var_idx_vec[operation_idx] = var_idx;
            lit_idx[operation_idx] = l_idx;
            operation_change_value_vec[operation_idx++] = change_value;
        }
    }
    
    int ls_solver::pick_critical_pair()
    {
        // return -1;
        int softflag = 0;
        std::vector<int> sc;
        if (unsat_clauses->size() > 8)
        {
            while (sc.size() < 8)
            {
                int tmp_clause = unsat_clauses->element_at(mt() % unsat_clauses->size());
                if (find(sc.begin(), sc.end(), tmp_clause) == sc.end())
                {
                    sc.push_back(tmp_clause);
                }
            }
        }
        else if (unsat_clauses->size() > 0)
        {
            for (int i = 0; i < unsat_clauses->size(); i++)
            {
                sc.push_back(unsat_clauses->element_at(i));
            }
        }
        else if (unsat_soft_clauses->size() > 15)
        {
            softflag = 1;
            while (sc.size() < 15)
            {
                int tmp_clause = unsat_soft_clauses->element_at(mt() % unsat_soft_clauses->size());
                if (find(sc.begin(), sc.end(), tmp_clause) == sc.end())
                {
                    sc.push_back(tmp_clause);
                }
            }
        }
        else
        {
            softflag = 1;
            for (int i = 0; i < unsat_soft_clauses->size(); i++)
            {
                sc.push_back(unsat_soft_clauses->element_at(i));
            }
        }
        int v1;
        __int128_t s1 = INT64_MIN;
        __int128_t s2 = INT64_MIN;
        std::vector<lit> FV;
        std::vector<__int128_t> FV_change_value;
        clause *cp;
        soft_clause *scp;
        int var_idx;
        __int128_t change_value;
        int operation_idx = 0;
        // int score;
        std::vector<int> pre_var;
        std::vector<__int128_t> pre_value;
        // std::vector<int>pre_score;
        for (int i = 0; i < sc.size(); i++)
        {
            if (softflag == 0)
            {
                cp = &(_clauses[sc[i]]);
                // for(int i=0;i<cp->lia_literals.size();i++)
                // {
                int randl = cp->lia_literals[mt() % (cp->lia_literals.size())];
                // int randl=cp->lia_literals[i];
                lit *l = &(_lits[std::abs(randl)]);
                for (int k = 0; k < l->pos_coff.size(); k++)
                {
                    var_idx = l->pos_coff_var_idx[k];
                    change_value = (randl > 0) ? devide(-l->delta, l->pos_coff[k]) : devide(1 - l->delta, l->pos_coff[k]);
                    // if((randl>0&&_step<tabulist[2*var_idx+1])||(randl<0&&_step<tabulist[2*var_idx])) continue;
                    if (is_in_bound(var_idx, change_value) == 0)
                        continue;
                    // if(is_in_bound(var_idx,change_value)==true)
                    // {
                    pre_var.push_back(var_idx);
                    pre_value.push_back(change_value);
                    // pre_score.push_back(critical_score(var_idx,change_value));
                    //}
                }
                for (int k = 0; k < l->neg_coff.size(); k++)
                {

                    var_idx = l->neg_coff_var_idx[k];
                    change_value = (randl > 0) ? devide(l->delta, l->neg_coff[k]) : devide(l->delta - 1, l->neg_coff[k]);
                    // if((randl>0&&_step<tabulist[2*var_idx])||(randl<0&&_step<tabulist[2*var_idx+1])) continue;
                    if (is_in_bound(var_idx, change_value) == 0)
                        continue;
                    // if(is_in_bound(var_idx,change_value)==true)
                    // {
                    pre_var.push_back(var_idx);
                    pre_value.push_back(change_value);
                    // pre_score.push_back(critical_score(var_idx,change_value));
                    //}
                }
                //}
            }
            else
            {
                scp = &(_soft_clauses[sc[i]]);
                int randl = scp->lia_literals[mt() % (scp->lia_literals.size())];
                lit *l = &(_lits[std::abs(randl)]);
                for (int k = 0; k < l->pos_coff.size(); k++)
                {
                    var_idx = l->pos_coff_var_idx[k];
                    change_value = (randl > 0) ? devide(-l->delta, l->pos_coff[k]) : devide(1 - l->delta, l->pos_coff[k]);
                    // if((randl>0&&_step<tabulist[2*var_idx+1])||(randl<0&&_step<tabulist[2*var_idx])) continue;
                    if (is_in_bound(var_idx, change_value) == 0)
                        continue;
                    pre_var.push_back(var_idx);
                    pre_value.push_back(change_value);
                    // pre_score.push_back(critical_score(var_idx,change_value));
                }
                for (int k = 0; k < l->neg_coff.size(); k++)
                {
                    var_idx = l->neg_coff_var_idx[k];
                    change_value = (randl > 0) ? devide(l->delta, l->neg_coff[k]) : devide(l->delta - 1, l->neg_coff[k]);
                    // if((randl>0&&_step<tabulist[2*var_idx])||(randl<0&&_step<tabulist[2*var_idx+1])) continue;
                    if (is_in_bound(var_idx, change_value) == 0)
                        continue;
                    pre_var.push_back(var_idx);
                    pre_value.push_back(change_value);
                    // pre_score.push_back(critical_score(var_idx,change_value));
                }
            }
        }
        // int best_score=INT16_MIN,best_var_1=-1,best_var_2;
        __int128_t best_change_1, best_change_2;
        std::vector<var_value> critical_pair;
        std::vector<var_value> critical_pair_2;
        for (int i = 0; i < pre_var.size(); i++)
        {
            // move to be added
            for (int j = 0; j < _vars[pre_var[i]].literals.size(); j++)
            {
                int l_idx = _vars[pre_var[i]].literals[j];
                lit *l = &(_lits[std::abs(l_idx)]);
                int var_coff = _vars[pre_var[i]].literal_coff[j];
                cp = &(_clauses[_vars[pre_var[i]].literal_clause[j]]);
                if (cp->sat_count != 1)
                    continue;
                if (l_idx > 0 && l->delta <= 0)
                {
                    __int128_t delta_change = pre_value[i] * var_coff + l->delta;
                    if (delta_change > 0)
                    {
                        for (int k = 0; k < l->pos_coff.size(); k++)
                        {
                            if (pre_var[i] == l->pos_coff_var_idx[k])
                                continue;
                            var_value cur;
                            cur.x = pre_var[i];
                            cur.value_x = pre_value[i];
                            cur.y = l->pos_coff_var_idx[k];
                            cur.value_y = devide(-delta_change, l->pos_coff[k]);
                            if (is_in_bound(cur.y, cur.value_y) == 0)
                                continue;
                            if (l->delta == 0)
                                critical_pair.push_back(cur);
                            else
                                critical_pair_2.push_back(cur);
                        }
                        for (int k = 0; k < l->neg_coff.size(); k++)
                        {
                            if (pre_var[i] == l->neg_coff_var_idx[k])
                                continue;
                            var_value cur;
                            cur.x = pre_var[i];
                            cur.value_x = pre_value[i];
                            cur.y = l->neg_coff_var_idx[k];
                            cur.value_y = devide(delta_change, l->neg_coff[k]);
                            if (is_in_bound(cur.y, cur.value_y) == 0)
                                continue;
                            if (l->delta == 0)
                                critical_pair.push_back(cur);
                            else
                                critical_pair_2.push_back(cur);
                        }
                    }
                }
                if (l_idx < 0 && l->delta >= 1)
                {
                    __int128_t delta_change = pre_value[i] * var_coff + l->delta;
                    if (delta_change <= 0)
                    {
                        for (int k = 0; k < l->pos_coff.size(); k++)
                        {
                            if (pre_var[i] == l->pos_coff_var_idx[k])
                                continue;
                            var_value cur;
                            cur.x = pre_var[i];
                            cur.value_x = pre_value[i];
                            cur.y = l->pos_coff_var_idx[k];
                            cur.value_y = devide(1 - delta_change, l->pos_coff[k]);
                            if (is_in_bound(cur.y, cur.value_y) == 0)
                                continue;
                            if (l->delta == 1)
                                critical_pair.push_back(cur);
                            else
                                critical_pair_2.push_back(cur);
                        }
                        for (int k = 0; k < l->neg_coff.size(); k++)
                        {
                            if (pre_var[i] == l->neg_coff_var_idx[k])
                                continue;
                            var_value cur;
                            cur.x = pre_var[i];
                            cur.value_x = pre_value[i];
                            cur.y = l->neg_coff_var_idx[k];
                            cur.value_y = devide(delta_change - 1, l->neg_coff[k]);
                            if (is_in_bound(cur.y, cur.value_y) == 0)
                                continue;
                            if (l->delta == 1)
                                critical_pair.push_back(cur);
                            else
                                critical_pair_2.push_back(cur);
                        }
                    }
                }
            }
            // soft to be revised
            for (int j = 0; j < _vars[pre_var[i]].soft_literals.size(); j++)
            {
                int l_idx = _vars[pre_var[i]].soft_literals[j];
                lit *l = &(_lits[std::abs(l_idx)]);
                int var_coff = _vars[pre_var[i]].soft_literal_coff[j];
                scp = &(_soft_clauses[_vars[pre_var[i]].soft_literal_clause[j]]);
                if (scp->sat_count != 1)
                    continue;
                if (l_idx > 0 && l->delta >= 0)
                {
                    __int128_t delta_change = pre_value[i] * var_coff + l->delta;
                    if (delta_change > 0)
                    {
                        for (int k = 0; k < l->pos_coff.size(); k++)
                        {
                            if (pre_var[i] == l->pos_coff_var_idx[k])
                                continue;
                            var_value cur;
                            cur.x = pre_var[i];
                            cur.value_x = pre_value[i];
                            cur.y = l->pos_coff_var_idx[k];
                            cur.value_y = devide(-delta_change, l->pos_coff[k]);
                            if (is_in_bound(cur.y, cur.value_y) == 0)
                                continue;
                            if (l->delta == 0)
                                critical_pair.push_back(cur);
                            else
                                critical_pair_2.push_back(cur);
                        }
                        for (int k = 0; k < l->neg_coff.size(); k++)
                        {
                            if (pre_var[i] == l->neg_coff_var_idx[k])
                                continue;
                            var_value cur;
                            cur.x = pre_var[i];
                            cur.value_x = pre_value[i];
                            cur.y = l->neg_coff_var_idx[k];
                            cur.value_y = devide(delta_change, l->neg_coff[k]);
                            if (is_in_bound(cur.y, cur.value_y) == 0)
                                continue;
                            if (l->delta == 0)
                                critical_pair.push_back(cur);
                            /*本来没有写else*/ else
                                critical_pair_2.push_back(cur);
                        }
                    }
                }
                if (l_idx < 0 && l->delta >= 1)
                {
                    __int128_t delta_change = pre_value[i] * var_coff + l->delta;
                    if (delta_change <= 0)
                    {
                        for (int k = 0; k < l->pos_coff.size(); k++)
                        {
                            if (pre_var[i] == l->pos_coff_var_idx[k])
                                continue;
                            var_value cur;
                            cur.x = pre_var[i];
                            cur.value_x = pre_value[i];
                            cur.y = l->pos_coff_var_idx[k];
                            cur.value_y = devide(1 - delta_change, l->pos_coff[k]);
                            if (is_in_bound(cur.y, cur.value_y) == 0)
                                continue;
                            if (l->delta == 1)
                                critical_pair.push_back(cur);
                            else
                                critical_pair_2.push_back(cur);
                        }
                        for (int k = 0; k < l->neg_coff.size(); k++)
                        {
                            if (pre_var[i] == l->neg_coff_var_idx[k])
                                continue;
                            var_value cur;
                            cur.x = pre_var[i];
                            cur.value_x = pre_value[i];
                            cur.y = l->neg_coff_var_idx[k];
                            cur.value_y = devide(delta_change - 1, l->neg_coff[k]);
                            if (is_in_bound(cur.y, cur.value_y) == 0)
                                continue;
                            if (l->delta == 1)
                                critical_pair.push_back(cur);
                            else
                                critical_pair_2.push_back(cur);
                        }
                    }
                }
            }
        }
        bool BMS;
        int cnt;
        int score;
        int best_score = 0;
        var_value best_pair;
        best_pair.x = -1;
        best_pair.y = -1;
        var_value cur;
        if (critical_pair.size() > 100) // 50
        {
            BMS = true;
            cnt = 100;
        }
        else
        {
            BMS = false;
            cnt = critical_pair.size();
        }
        for (int i = 0; i < cnt; i++)
        {
            if (BMS)
            {
                int idx = mt() % (critical_pair.size() - i);
                score = critical_score_pair(critical_pair[idx].x, critical_pair[idx].value_x, critical_pair[idx].y, critical_pair[idx].value_y);
                cur = critical_pair[idx];
                critical_pair[idx] = critical_pair[critical_pair.size() - i - 1];
            }
            else
            {
                score = critical_score_pair(critical_pair[i].x, critical_pair[i].value_x, critical_pair[i].y, critical_pair[i].value_y);
                cur = critical_pair[i];
            }
            // std::cout<<"score : "<<score<<std::endl;
            if (score > best_score)
            {
                // std::cout<<score<<"  "<<best_score;
                best_score = score;
                best_pair = cur;
            }
        }
        if (best_pair.x != -1 && best_pair.y != -1)
        {
            // std::cout<<best_pair.x<<std::endl;
            critical_move(best_pair.x, best_pair.value_x);
            critical_move(best_pair.y, best_pair.value_y);
            // std::cout<<"score : "<<best_score<<" ";
            // std::cout<<best_pair.x<<"  "<<print_128(best_pair.value_x)<<"  "<<best_pair.y<<"   "<<print_128(best_pair.value_y)<<std::endl;
            update_clause_weight();
            return 1;
        }

        // two level
        if (critical_pair_2.size() > 50)
        {
            BMS = true;
            cnt = 50;
        }
        else
        {
            BMS = false;
            cnt = critical_pair_2.size();
        }
        for (int i = 0; i < cnt; i++)
        {
            if (BMS)
            {
                int idx = mt() % (critical_pair_2.size() - i);
                score = critical_score_pair(critical_pair_2[idx].x, critical_pair_2[idx].value_x, critical_pair_2[idx].y, critical_pair_2[idx].value_y);
                cur = critical_pair_2[idx];
                critical_pair_2[idx] = critical_pair_2[critical_pair_2.size() - i - 1];
            }
            else
            {
                score = critical_score_pair(critical_pair_2[i].x, critical_pair_2[i].value_x, critical_pair_2[i].y, critical_pair_2[i].value_y);
                cur = critical_pair_2[i];
            }
            // std::cout<<"score : "<<score<<std::endl;
            if (score > best_score)
            {
                // std::cout<<score<<"  "<<best_score;
                best_score = score;
                best_pair = cur;
            }
        }
        if (best_pair.x != -1 && best_pair.y != -1)
        {
            // std::cout<<best_pair.x<<std::endl;
            critical_move(best_pair.x, best_pair.value_x);
            critical_move(best_pair.y, best_pair.value_y);
            // std::cout<<"score : "<<best_score<<" ";
            // std::cout<<best_pair.x<<"  "<<print_128(best_pair.value_x)<<"  "<<best_pair.y<<"   "<<print_128(best_pair.value_y)<<std::endl;
            update_clause_weight();
            return 1;
        }

        // two level
        return -1; // bool?
    }

    void ls_solver::print_solution()
    {
        print_mv();
    }
}
