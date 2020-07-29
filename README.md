# Alghoritms and data structure project 2019

Final examination of the algorithms course at Politecnico di Milano.
The project consists in a handler system that tracks multiple types of relations between entities, it is designed to optimize both time and space complexity and it passes all the constraints tests given. Reb-Black trees and dynamic lists are implemented and used to manipulate and memorize the informations, C language is used to have full control of the code and data structures.

## User commands

Once launched the program, users can give the following inputs:

Command | input
------------ | -------------
addent | *'id of entity to add'*
delent | *'id of entity to delete*
addrel | *'id of origin entity'*  *'id of destination entity'*   *'id of relation to create'*
delrel | *'id of origin entity of the rel'*   *'id of destination entity of the rel'*   *'id of the rel'*
report | -
end | -

report give in output the list of existing relations and for each show the entity/ies with the max number of that relation with other entities
