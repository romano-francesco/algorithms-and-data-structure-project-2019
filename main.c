//
//  main.c
//  API_progetto
//
//  Created by Francesco on 12/07/2019.
//  Copyright © 2019 Francesco. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define N 1000

// Type definition and used structures

typedef char *String;

typedef struct nodo1{
    String id_linked;
    char color;           // R is red, B is black
    struct nodo1 *p;
    struct nodo1 *left;
    struct nodo1 *right;
}inRelWith;

typedef inRelWith *linked_nodes;  //Root of related tree

typedef struct relNode{
    String rel_name;
    int entryCounter;
    struct relNode *next;
    linked_nodes linked_tree;
}elem;

typedef elem *relations;  //List of relations of a node

typedef struct relMax{
    String maxEntryEntity;
    struct relMax *next;
}rel_max;

typedef rel_max *max_list;  //List of entities which have max entry of a type of relation

typedef struct relList{ //Struscture used to track monitored relations
    String rel_name;
    int entryCounter;
    max_list max;
    struct relList *next;
}rel_list;

typedef rel_list *rel_monitorate;

typedef struct nodo2{
    String id_ent;
    char color;
    struct nodo2 *p;
    struct nodo2 *left;
    struct nodo2 *right;
    relations rel_list;
}entita;

typedef entita *entita_tree;  //Root of entity tree

//Usefull global variables

entita nil;
entita_tree T_nil = &nil;
inRelWith nil_linked;
linked_nodes T_nil_linked = &nil_linked;

//Support methods

char * create_name(String toAlloc){   //Function that allocates dynamic strings for entity names
    unsigned long len = strlen(toAlloc) + 1;
    char *name;
    name = malloc((len * sizeof(char)));
    strcpy(name, toAlloc);
    return name;
}

relations ins_in_testa(relations list,String id_rel){ //Insertion in head in the relations list
    relations new_rel;
    new_rel = malloc(sizeof(elem));
    new_rel->rel_name = id_rel;
    new_rel->entryCounter = 0;
    new_rel->linked_tree = T_nil_linked;
    new_rel->next = list;
    return new_rel;
}

relations search_in_list(relations list, String id_rel){
    relations tmp = list;
    while(tmp != NULL){
        if(strcmp(tmp->rel_name, id_rel) == 0)
            return tmp;
        tmp = tmp->next;
    }
    return tmp;
}

rel_monitorate ins_in_order(rel_monitorate list, String id_rel){ //In order insertion in the monitored relations list
    rel_monitorate new_rel, corr, prec;
    prec = NULL;
    corr = list;
    while(corr != NULL && strcoll(corr->rel_name, id_rel) < 0){
        prec = corr;
        corr = corr->next;
    }
    new_rel = malloc(sizeof(rel_list));
    new_rel->rel_name = id_rel;
    new_rel->entryCounter = 0;
    //new_rel->flag = 'N';
    new_rel->max = NULL;
    new_rel->next = corr;
    if(prec != NULL){
        prec->next = new_rel;
        return list;
    }
    else
        return new_rel;
}

rel_monitorate search_in_list_monitorate(rel_monitorate list, String id_rel){
    rel_monitorate tmp = list;
    while(tmp != NULL){
        if(strcmp(tmp->rel_name, id_rel) == 0)
            return tmp;
        tmp = tmp->next;
    }
    return tmp;
}

max_list ins_in_order_max(max_list list, String max_entry_entity){ //In order insertion in the list of entities with max counter
    max_list new_max, corr, prec;
    prec = NULL;
    corr = list;
    while(corr != NULL && strcoll(corr->maxEntryEntity, max_entry_entity) < 0){
        prec = corr;
        corr = corr->next;
    }
    new_max = malloc(sizeof(rel_max));
    new_max->maxEntryEntity = max_entry_entity;
    new_max->next = corr;
    if(prec != NULL){
        prec->next = new_max;
        return list;
    }
    else
        return new_max;
}

void update_counter_rel_monitorate_add(rel_monitorate list, relations counter, String id_entita){ //Update counters in relations monitored
    rel_monitorate toUpdate = search_in_list_monitorate(list, counter->rel_name);
    if(toUpdate != NULL){
        if(counter->entryCounter > toUpdate->entryCounter){
            toUpdate->entryCounter = counter->entryCounter;
            //free(toUpdate->max);
            toUpdate->max = NULL;
            toUpdate->max = ins_in_order_max(toUpdate->max, id_entita);
        }
        else if(counter->entryCounter == toUpdate->entryCounter){
            toUpdate->max = ins_in_order_max(toUpdate->max, id_entita);
        }
    }
}

void new_max_list(entita_tree x, String id_rel, rel_monitorate toUpdate){ //Tree visit and creation of new list max
    if(x != T_nil){
        relations toCheck = search_in_list(x->rel_list, id_rel);
        if(toCheck != NULL){
            if(toCheck->entryCounter == toUpdate->entryCounter){
                toUpdate->max = ins_in_order_max(toUpdate->max, x->id_ent);
            }
        }
        new_max_list(x->left, id_rel, toUpdate);
        new_max_list(x->right, id_rel, toUpdate);
    }
}

max_list delete_max(max_list list, String toDel){
    max_list temp;
    if(list != NULL){
        if(strcmp(list->maxEntryEntity, toDel) == 0){
            temp = list->next;
            free(list);
            return temp;
        }
        else{
            list->next = delete_max(list->next, toDel);
            return list;
        }
    }
    else return list;
}

void update_counter_rel_monitorate_del(entita_tree *T, rel_monitorate list, relations counter, String id_entita){
    rel_monitorate toUpdate = search_in_list_monitorate(list, counter->rel_name);
    if((counter->entryCounter == toUpdate->entryCounter - 1) && toUpdate->max->next == NULL){//If maximum is decremented, then all max will be changed
        toUpdate->entryCounter--;
        toUpdate->max = NULL;
        new_max_list(*T, counter->rel_name, toUpdate);
    }
    else if(counter->entryCounter == toUpdate->entryCounter - 1){
        toUpdate->max = delete_max(toUpdate->max, id_entita);
    }
}

//Implementation of RB-tree for relationed

linked_nodes rb_search_linked(linked_nodes node_x, String toFind){
    if(node_x == T_nil_linked || strcmp(toFind, node_x->id_linked) == 0){
        return node_x;
    }
    else if(strcoll(toFind, node_x->id_linked) < 0)
        return rb_search_linked(node_x->left, toFind);
    else return rb_search_linked(node_x->right, toFind);
}

void left_rotate_linked(linked_nodes *T, linked_nodes node_x){
    linked_nodes node_y;
    node_y = node_x->right;
    node_x->right = node_y->left;
    if(node_y->left != T_nil_linked)
        node_y->left->p = node_x;
    node_y->p = node_x->p;
    if(node_x->p == T_nil_linked)
        *T = node_y;
    else if(node_x == node_x->p->left)
        node_x->p->left = node_y;
    else node_x->p->right = node_y;
    node_y->left = node_x;
    node_x->p = node_y;
}

void right_rotate_linked(linked_nodes *T, linked_nodes node_x){
    linked_nodes node_y;
    node_y = node_x->left;
    node_x->left = node_y->right;
    if(node_y->right != T_nil_linked)
        node_y->right->p = node_x;
    node_y->p = node_x->p;
    if(node_x->p == T_nil_linked)
        *T = node_y;
    else if(node_x == node_x->p->right)
        node_x->p->right = node_y;
    else node_x->p->left = node_y;
    node_y->right = node_x;
    node_x->p = node_y;
}

void rb_insert_fixup_linked(linked_nodes *T, linked_nodes node_z){
    linked_nodes node_x;
    linked_nodes node_y;
    if(node_z == *T)
        (*T)->color = 'B';
    else {
        node_x = node_z->p;
        if(node_x->color == 'R'){
            if(node_x == node_x->p->left){
                node_y = node_x->p->right;
                if(node_y->color == 'R'){
                    node_x->color = 'B';
                    node_y->color = 'B';
                    node_x->p->color = 'R';
                    rb_insert_fixup_linked(T, node_x->p);
                }
                else{
                    if(node_z == node_x->right){
                        node_z = node_x;
                        left_rotate_linked(T, node_z);
                        node_x = node_z->p;
                    }
                    node_x->color = 'B';
                    node_x->p->color = 'R';
                    right_rotate_linked(T, node_x->p);
                }
            }
            else{
                node_y = node_x->p->left;
                if(node_y->color == 'R'){
                    node_x->color = 'B';
                    node_y->color = 'B';
                    node_x->p->color = 'R';
                    rb_insert_fixup_linked(T, node_x->p);
                }
                else{
                    if(node_z == node_x->left){
                        node_z = node_x;
                        right_rotate_linked(T, node_z);
                        node_x = node_z->p;
                    }
                    node_x->color = 'B';
                    node_x->p->color = 'R';
                    left_rotate_linked(T, node_x->p);
                }
            }
        }
    }
    (*T)->color = 'B';
}

void rb_insert_linked(linked_nodes *T, linked_nodes node_z){
    linked_nodes node_y = T_nil_linked;
    linked_nodes node_x = *T;
    while(node_x != T_nil_linked){
        node_y = node_x;
        if(strcmp(node_z->id_linked, node_x->id_linked) == 0){
            return;
        }
        if(strcoll(node_z->id_linked, node_x->id_linked) < 0)
            node_x = node_x->left;
        else
            node_x = node_x->right;
    }
    node_z->p = node_y;
    if(node_y == T_nil_linked)
        *T = node_z;
    else if(strcoll(node_z->id_linked, node_y->id_linked) < 0)
        node_y->left = node_z;
    else
        node_y->right = node_z;
    node_z->left = T_nil_linked;
    node_z->right = T_nil_linked;
    node_z->color = 'R';
    rb_insert_fixup_linked(T, node_z);
}

linked_nodes tree_minimum_linked(linked_nodes node_x){
    while(node_x->left != T_nil_linked){
        node_x = node_x->left;
    }
    return node_x;
}

linked_nodes tree_successor_linked(linked_nodes node_x){
    linked_nodes node_y;
    if(node_x->right != T_nil_linked)
        return tree_minimum_linked(node_x->right);
    node_y = node_x->p;
    while(node_y != T_nil_linked && node_x == node_y->right){
        node_x = node_y;
        node_y = node_y->p;
    }
    return node_y;
}

void rb_delete_fixup_linked(linked_nodes *T, linked_nodes node_x){
    linked_nodes node_w;
    if(node_x->color == 'R' || node_x->p == T_nil_linked)
        node_x->color = 'B';
    else if(node_x == node_x->p->left){
        node_w = node_x->p->right;
        if(node_w->color == 'R'){
            node_w->color = 'B';
            node_x->p->color = 'R';
            left_rotate_linked(T, node_x->p);
            node_w = node_x->p->right;
        }
        if(node_w->left->color == 'B' && node_w->right->color == 'B'){
            node_w->color = 'R';
            rb_delete_fixup_linked(T, node_x->p);
        }
        else{
            if(node_w->right->color == 'B'){
                node_w->left->color = 'B';
                node_w->color = 'R';
                right_rotate_linked(T, node_w);
                node_w = node_x->p->right;
            }
            node_w->color = node_x->p->color;
            node_x->p->color = 'B';
            node_w->right->color = 'B';
            left_rotate_linked(T, node_x->p);
        }
    }
    else{
        node_w = node_x->p->left;
        if(node_w->color == 'R'){
            node_w->color = 'B';
            node_x->p->color = 'R';
            right_rotate_linked(T, node_x->p);
            node_w = node_x->p->left;
        }
        if(node_w->right->color == 'B' && node_w->left->color == 'B'){
            node_w->color = 'R';
            rb_delete_fixup_linked(T, node_x->p);
        }
        else{
            if(node_w->left->color == 'B'){
                node_w->right->color = 'B';
                node_w->color = 'R';
                left_rotate_linked(T, node_w);
                node_w = node_x->p->left;
            }
            node_w->color = node_x->p->color;
            node_x->p->color = 'B';
            node_w->left->color = 'B';
            right_rotate_linked(T, node_x->p);
        }
    }
}

void rb_delete_linked(linked_nodes *T, linked_nodes node_z){
    linked_nodes node_y;
    linked_nodes node_x;
    if(node_z->left == T_nil_linked || node_z->right == T_nil_linked)
        node_y = node_z;
    else
        node_y = tree_successor_linked(node_z);
    if(node_y->left != T_nil_linked)
        node_x = node_y->left;
    else
        node_x = node_y->right;
    node_x->p = node_y->p;
    if(node_y->p == T_nil_linked)
        *T = node_x;
    else if(node_y == node_y->p->left)
        node_y->p->left = node_x;
    else
        node_y->p->right = node_x;
    if(node_y != node_z)
        node_z->id_linked = node_y->id_linked;
    if(node_y->color == 'B')
        rb_delete_fixup_linked(T, node_x);
    free(node_y);
}

//Implementation of RB- tree for entities

entita_tree rb_search(entita_tree node_x, String toFind){
    if(node_x == T_nil || strcmp(toFind, node_x->id_ent) == 0)
        return node_x;
    if(strcoll(toFind, node_x->id_ent) < 0)
        return rb_search(node_x->left, toFind);
    else return rb_search(node_x->right, toFind);
}

void left_rotate(entita_tree *T, entita_tree node_x){
    entita_tree node_y;
    node_y = node_x->right;
    node_x->right = node_y->left;
    if(node_y->left != T_nil)
        node_y->left->p = node_x;
    node_y->p = node_x->p;
    if(node_x->p == T_nil)
        *T = node_y;
    else if(node_x == node_x->p->left)
        node_x->p->left = node_y;
    else
        node_x->p->right = node_y;
    node_y->left = node_x;
    node_x->p = node_y;
}

void right_rotate(entita_tree *T, entita_tree node_x){
    entita_tree node_y;
    node_y = node_x->left;
    node_x->left = node_y->right;
    if(node_y->right != T_nil)
        node_y->right->p = node_x;
    node_y->p = node_x->p;
    if(node_x->p == T_nil)
        *T = node_y;
    else if(node_x == node_x->p->right)
        node_x->p->right = node_y;
    else
        node_x->p->left = node_y;
    node_y->right = node_x;
    node_x->p = node_y;
}

void rb_insert_fixup(entita_tree *T, entita_tree node_z){
    entita_tree node_x;
    entita_tree node_y;
    if(node_z == *T)
        (*T)->color = 'B';
    else {
        node_x = node_z->p;
        if(node_x->color == 'R'){
            if(node_x == node_x->p->left){
                node_y = node_x->p->right;
                if(node_y->color == 'R'){
                    node_x->color = 'B';
                    node_y->color = 'B';
                    node_x->p->color = 'R';
                    rb_insert_fixup(T, node_x->p);
                }
                else{
                    if(node_z == node_x->right){
                        node_z = node_x;
                        left_rotate(T, node_z);
                        node_x = node_z->p;
                    }
                    node_x->color = 'B';
                    node_x->p->color = 'R';
                    right_rotate(T, node_x->p);
                }
            }
            else{
                node_y = node_x->p->left;
                if(node_y->color == 'R'){
                    node_x->color = 'B';
                    node_y->color = 'B';
                    node_x->p->color = 'R';
                    rb_insert_fixup(T, node_x->p);
                }
                else{
                    if(node_z == node_x->left){
                        node_z = node_x;
                        right_rotate(T, node_z);
                        node_x = node_z->p;
                    }
                    node_x->color = 'B';
                    node_x->p->color = 'R';
                    left_rotate(T, node_x->p);
                }
            }
        }
    }
    (*T)->color = 'B';
}

void rb_insert(entita_tree *T, entita_tree node_z){
    entita_tree node_y = T_nil;
    entita_tree node_x = *T;
    while(node_x != T_nil){
        node_y = node_x;
        if(strcmp(node_z->id_ent, node_x->id_ent) == 0){
            return;
        }
        if(strcoll(node_z->id_ent, node_x->id_ent) < 0)
            node_x = node_x->left;
        else
            node_x = node_x->right;
    }
    node_z->p = node_y;
    if(node_y == T_nil)
        *T = node_z;
    else if(strcoll(node_z->id_ent, node_y->id_ent) < 0)
        node_y->left = node_z;
    else
        node_y->right = node_z;
    node_z->left = T_nil;
    node_z->right = T_nil;
    node_z->color = 'R';
    rb_insert_fixup(T, node_z);
}

entita_tree tree_minimum(entita_tree node_x){
    while(node_x->left != T_nil){
        node_x = node_x->left;
    }
    return node_x;
}

entita_tree tree_successor(entita_tree node_x){
    entita_tree node_y;
    if(node_x->right != T_nil)
        return tree_minimum(node_x->right);
    node_y = node_x->p;
    while(node_y != T_nil && node_x == node_y->right){
        node_x = node_y;
        node_y = node_y->p;
    }
    return node_y;
}

void rb_delete_fixup(entita_tree *T, entita_tree node_x){
    entita_tree node_w;
    if(node_x->color == 'R' || node_x->p == T_nil)
        node_x->color = 'B';
    else if(node_x == node_x->p->left){
        node_w = node_x->p->right;
        if(node_w->color == 'R'){
            node_w->color = 'B';
            node_x->p->color = 'R';
            left_rotate(T, node_x->p);
            node_w = node_x->p->right;
        }
        if(node_w->left->color == 'B' && node_w->right->color == 'B'){
            node_w->color = 'R';
            rb_delete_fixup(T, node_x->p);
        }
        else {
            if (node_w->right->color == 'B') {
                node_w->left->color = 'B';
                node_w->color = 'R';
                right_rotate(T, node_w);
                node_w = node_x->p->right;
            }
            node_w->color = node_x->p->color;
            node_x->p->color = 'B';
            node_w->right->color = 'B';
            left_rotate(T, node_x->p);
        }
    }
    else{
        node_w = node_x->p->left;
        if(node_w->color == 'R'){
            node_w->color = 'B';
            node_x->p->color = 'R';
            right_rotate(T, node_x->p);
            node_w = node_x->p->left;
        }
        if(node_w->right->color == 'B' && node_w->left->color == 'B'){
            node_w->color = 'R';
            rb_delete_fixup(T, node_x->p);
        }
        else{
            if(node_w->left->color == 'B'){
                node_w->right->color = 'B';
                node_w->color = 'R';
                left_rotate(T, node_w);
                node_w = node_x->p->left;
            }
            node_w->color = node_x->p->color;
            node_x->p->color = 'B';
            node_w->left->color = 'B';
            right_rotate(T, node_x->p);
        }
    }
}

void rb_delete(entita_tree *T, entita_tree node_z){
    entita_tree node_y;
    entita_tree node_x;
    if(node_z->left == T_nil || node_z->right == T_nil)
        node_y = node_z;
    else
        node_y = tree_successor(node_z);
    if(node_y->left != T_nil)
        node_x = node_y->left;
    else
        node_x = node_y->right;
    node_x->p = node_y->p;
    if(node_y->p == T_nil)
        *T = node_x;
    else if(node_y == node_y->p->left)
        node_y->p->left = node_x;
    else
        node_y->p->right = node_x;
    if(node_y != node_z) {
        node_z->id_ent = create_name(node_y->id_ent);
        node_z->rel_list = node_y->rel_list;
    }
    if(node_y->color == 'B')
        rb_delete_fixup(T, node_x);
    node_y->id_ent = NULL;
    free(node_y->id_ent);
    free(node_y);
}

//USER COMMANDS

//ADDENT IMPLEMENTATION
void addent(entita_tree *T, String id_ent){
    entita_tree node_z = malloc(sizeof(entita));
    node_z->id_ent = id_ent;
    node_z->color = 'R';
    node_z->p = T_nil;
    node_z->left = T_nil;
    node_z->right = T_nil;
    node_z->rel_list = NULL;
    rb_insert(T, node_z);
}

//ADDREL IMPLEMENTATION
rel_monitorate addrel(rel_monitorate L, entita_tree *T, String id_orig, String id_dest, String id_rel){
    entita_tree node_to_rel = rb_search(*T, id_orig);  //evaluates if relation is to be added in entity node
    entita_tree node_to_be_rel = rb_search(*T, id_dest);
    if(node_to_rel != T_nil && node_to_be_rel != T_nil){
        String entita_name_pointer = node_to_be_rel->id_ent;
        int isToAdd = 1; //flag to add in rel monitorate
        int isToAddInNode = 1; //flag to add in rel of the node
        String rel_name_pointer = NULL; //pointer to the name string
        rel_monitorate tmp = L;
        while(tmp != NULL){ //evaluates if rel has to be added in rel_monitorate
            if(strcmp(tmp->rel_name, id_rel) == 0){
                rel_name_pointer = tmp->rel_name;
                isToAdd = 0;
                break;
            }
            tmp = tmp->next;
        }
        if(isToAdd == 1){
            rel_name_pointer = create_name(id_rel);
            L = ins_in_order(L, rel_name_pointer);
        }
        relations temp = node_to_rel->rel_list;  //in this case the relation is added
        while(temp != NULL){
            if(strcmp(temp->rel_name, id_rel) == 0){
                isToAddInNode = 0;
                break;
            }
            temp = temp->next;
        }
        if(isToAddInNode == 0){ //check if the relation already exists
            if(rb_search_linked(temp->linked_tree, entita_name_pointer) != T_nil_linked){
                return L;
            }
        }
        if(isToAddInNode == 1){
            node_to_rel->rel_list = ins_in_testa(node_to_rel->rel_list, rel_name_pointer);
            temp = node_to_rel->rel_list;
        }
        linked_nodes node_linked = malloc(sizeof(inRelWith));
        node_linked->color = 'R';
        node_linked->id_linked = entita_name_pointer;
        node_linked->left = T_nil_linked;
        node_linked->right = T_nil_linked;
        node_linked->p = T_nil_linked;
        rb_insert_linked(&(temp->linked_tree), node_linked);

        //Counters update
        relations counter = search_in_list(node_to_be_rel->rel_list, rel_name_pointer);
        if(counter == NULL){
            node_to_be_rel->rel_list = ins_in_testa(node_to_be_rel->rel_list, rel_name_pointer);
            counter = node_to_be_rel->rel_list;
        }
        counter->entryCounter++;
        //Max counters update
        update_counter_rel_monitorate_add(L, counter, node_to_be_rel->id_ent);
    }
    return L;
}

//DELREL IMPLEMENTATION
void delrel(rel_monitorate L, entita_tree *T, String id_orig, String id_dest, String id_rel){
    entita_tree node_orig = rb_search(*T, id_orig);
    entita_tree node_dest = rb_search(*T, id_dest);
    if(node_orig != T_nil && node_dest != T_nil){
        relations rel_to_del = search_in_list(node_orig->rel_list, id_rel);
        if(rel_to_del != NULL){
            linked_nodes linked_to_del = rb_search_linked(rel_to_del->linked_tree, id_dest);
            if(linked_to_del != T_nil_linked){
                rb_delete_linked(&(rel_to_del->linked_tree), linked_to_del);
                //Decremento contatori
                relations counter = search_in_list(node_dest->rel_list, id_rel);
                if(counter == NULL){
                    node_dest->rel_list = ins_in_testa(node_dest->rel_list, id_rel);
                    counter = node_dest->rel_list;
                }
                if(counter->entryCounter > 0)
                    counter->entryCounter--;
                //Aggiorno contatori massimi
                update_counter_rel_monitorate_del(T, L, counter, node_dest->id_ent);
            }
        }
    }
}

//Delent support methods

void delrel_with_nodes(rel_monitorate L, entita_tree *T, entita_tree orig, entita_tree dest, relations rel){
    if(orig != T_nil){
        if(rel != NULL){
            linked_nodes linked_to_del = rb_search_linked(rel->linked_tree, dest->id_ent);
            if(linked_to_del != T_nil_linked){
                rb_delete_linked(&(rel->linked_tree), linked_to_del);
                //Counter decrement
                relations counter = search_in_list(dest->rel_list, rel->rel_name);
                if(counter == NULL){
                    dest->rel_list = ins_in_testa(dest->rel_list, rel->rel_name);
                    counter = dest->rel_list;
                }
                if(counter->entryCounter > 0)
                    counter->entryCounter--;
                //Update max counters
                update_counter_rel_monitorate_del(T, L, counter, dest->id_ent);
            }
        }
    }
}

void delete_exit_rel(rel_monitorate L, entita_tree *T, relations tmp, linked_nodes y, entita_tree x_to_del){
    if(y != T_nil_linked){
        delete_exit_rel(L, T, tmp, y->left, x_to_del);
        delete_exit_rel(L, T, tmp, y->right, x_to_del);
        entita_tree entita_y = rb_search(*T, y->id_linked);
        if(entita_y != T_nil){
            relations counter = search_in_list(entita_y->rel_list, tmp->rel_name);
            if(counter == NULL){
                entita_y->rel_list = ins_in_testa(entita_y->rel_list, tmp->rel_name);
                counter = entita_y->rel_list;
            }
            if(counter->entryCounter > 0)
                counter->entryCounter--;
            //Update max counters
            update_counter_rel_monitorate_del(T, L, counter, entita_y->id_ent);
        }
        free(y);
    }
}

void delete_all_toDel_rel(rel_monitorate L, entita_tree x, entita_tree *T, entita_tree node_to_del){
    if(x != T_nil){
        delete_all_toDel_rel(L, x->left, T, node_to_del);
        if(x != node_to_del){ //cancello relazioni entranti
            relations tmp = x->rel_list;
            while(tmp != NULL){
                delrel_with_nodes(L, T, x, node_to_del, tmp);
                tmp = tmp->next;
            }
        }
        else if(x == node_to_del){ //cancello relazioni uscenti
            relations temp = x->rel_list;
            while(temp != NULL){
                delete_exit_rel(L, T, temp, temp->linked_tree, x);
                temp = temp->next;
            }
        }
        delete_all_toDel_rel(L, x->right, T, node_to_del);
    }
}

void delent(rel_monitorate L, entita_tree *T, String id_to_del){
    entita_tree node_to_del = rb_search(*T, id_to_del);
    if(node_to_del != T_nil){
        delete_all_toDel_rel(L, *T, T, node_to_del);
        rb_delete(T, node_to_del);
    }
}

//REPORT IMPLEMENTATION
void report(rel_monitorate L){
    rel_monitorate  tmp = L;
    while(tmp != NULL){
        max_list tmp_max = tmp->max;
        if(tmp_max != NULL && tmp->entryCounter > 0){
            fputs(tmp->rel_name, stdout);
            while(tmp_max != NULL){
                fputs(" ", stdout);
                fputs(tmp_max->maxEntryEntity, stdout);
                tmp_max = tmp_max->next;
            }
            printf(" %d; ", tmp->entryCounter);
        }
        tmp = tmp->next;
    }
}

// MAIN

int main(int argc, const char * argv[]) {
    int running = 1;
    char BUFFER[N];
    char delim[] = " \n";
    char *command;
    char *entita1;
    char *entita2;
    char *relation;
    //Sentinel nodes setted
    T_nil->left = T_nil;
    T_nil->p = T_nil;
    T_nil->right = T_nil;
    T_nil->rel_list = NULL;
    T_nil->id_ent = NULL;
    T_nil->color = 'B';
    T_nil_linked->left = T_nil_linked;
    T_nil_linked->p = T_nil_linked;
    T_nil_linked->right = T_nil_linked;
    T_nil_linked->id_linked = NULL;
    T_nil_linked->color = 'B';
    //Structures initialization
    entita_tree T = T_nil;
    rel_monitorate L = NULL;
    //Main loop for user inputs
    while(running == 1){
        fgets(BUFFER, N, stdin);
        command = strtok(BUFFER, delim);
        if(command == NULL)
            continue;
        //ADDENT
        if(strcmp(command, "addent") == 0){
            entita1 = strtok(NULL, delim);
            addent(&T, create_name(entita1));
        }
        //DELENT
        if(strcmp(command, "delent") == 0){
            entita1 = strtok(NULL, delim);
            delent(L, &T, entita1);
        }
        //ADDREL
        if(strcmp(command, "addrel") == 0){
            entita1 = strtok(NULL, delim);
            entita2 = strtok(NULL, delim);
            relation = strtok(NULL, delim);
            L = addrel(L, &T, entita1, entita2, relation);
        }
        //DELREL
        if(strcmp(command, "delrel") == 0){
            entita1 = strtok(NULL, delim);
            entita2 = strtok(NULL, delim);
            relation = strtok(NULL, delim);
            delrel(L, &T, entita1, entita2, relation);
        }
        //REPORT
        if(strcmp(command, "report") == 0){
            if(L == NULL || L->next == NULL && L->entryCounter == 0)
                printf("none");
            else
                report(L);
            printf("\n");
        }
        //END
        if(strcmp(command, "end") == 0){
            //printf("\n");
            running = 0;
        }
    }
    return 0;
}
