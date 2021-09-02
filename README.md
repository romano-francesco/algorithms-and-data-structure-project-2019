# Algoritms and data structure project 2019

Final examination of the algorithms course at Politecnico di Milano.
The project consists in a handler system that tracks multiple types of relations between entities, it is designed to optimize both time and space complexity and it passes all the constraints tests given. Red-Black trees and dynamic lists are implemented from scratch and used to manipulate and memorize the informations, C language is used to have full control of the code and data structures.

## User commands

Once launched the program, users can give the following commands and inputs:

Command | input | result
------------ | ------------- | -------------
addent | *'id of entity to add'* | add an entity to the tracking system
delent | *'id of entity to delete'* | delete an entity from the system
addrel | *'id of origin entity'*  *'id of destination entity'*   *'id of relation to create'* | add an arbitrary relation between two entities in the system
delrel | *'id of origin entity of the rel'*   *'id of destination entity of the rel'*   *'id of the rel'* | delete an existing relation between entities
report | - | gives in output the list of existing relations and for each show the entity/ies with the max number of that relation with other entities
end | - | terminate the session

