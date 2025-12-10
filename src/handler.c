#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "raylib.h"
#include "handler.h"

declare_component_pool(transforms, comp_Transform);

char *comp_names[COMP_TYPE_COUNT] = {
	"transform",
	"sprite",
};

void HandlerInit(Handler *handler, float dt) {
	_pool_transforms_init();

	// Allocate memory for entities
	handler->entity_count = 0;
	handler->entities = calloc(ENTITY_CAP, sizeof(Entity));
	handler->comp_mappings = calloc(ENTITY_CAP, sizeof(ComponentMap));

	for(int i = 0; i < 30; i++) { 
		SpawnEntity( 
			handler, (comp_Transform) { 
				.position = (Vector2){ 30 + (i * 30), 300},
				.velocity = (Vector2){ 0, 0 },
				.scale = 1, 
				.rotation = 0 
			} 
		);
		
		PrintComponentMappings(handler, i);
	}
}

void HandlerUpdate(Handler *handler, float dt) {
	TransformsUpdate(handler, dt);
}

void HandlerDraw(Handler *handler) {
	DrawText(TextFormat("transform_count: %d", _pool_transforms.count), 100, 100, 30, RAYWHITE);

	for(INT_N i = 0; i < _pool_transforms.count; i++) {
		comp_Transform *transform = &_pool_transforms.data[i];

		DrawCircleV(transform->position, 10, BLUE);
		DrawCircleV(transform->position, 5, BLACK);
	}
}

INT_N AddEntity(Handler *handler, uint32_t components) {
	// Initialize component mappings for new entity
	// By default, all entries map to nothing
	INT_N mappings[COMP_TYPE_COUNT] = { 0 };
	memset(mappings, COMP_NULL, sizeof(mappings));

	// Create new components and register their IDs to the mapping  
	for(uint32_t i = 0; i < COMP_TYPE_COUNT; i++) {
		uint32_t bit = (1 << i);

		//if(comp_names[i]) printf("checking for %s component \n", comp_names[i]);

		if(!(components & bit)) continue;
		//printf("%s component found!\n", comp_names[i]);

		switch(bit) {
			case COMP_TRANSFORM: {
				// Create empty transform
				comp_Transform new_transform = (comp_Transform) { 0 };

				// Add transform to pool 
				INT_N transform_id = _pool_transforms_add(new_transform);

				// Add transform's id to entity component mappings 
				mappings[i] = transform_id;

			} break;
		}	
	}	
	
	// Initialize entity struct 
	Entity new_entity = (Entity) {
		new_entity.components = components,
		new_entity.id = handler->entity_count++,
		new_entity.flags = 0
	};

	// Copy component mappings
	memcpy(new_entity.comp_map.component_id, mappings, sizeof(mappings));

	// Copy new entity to array
	handler->entities[new_entity.id] = new_entity;

	// Return index
	return new_entity.id;
}

// Create a new entity 
// Make, bind and map specified transform component 
void SpawnEntity(Handler *handler, comp_Transform transform) {
	// Initialize entity, insert to entity array
	INT_N id = AddEntity(handler, (COMP_TRANSFORM));

	// Get pointer to newly created entity 
	Entity *spawned_entity = &handler->entities[id];

	// Get transform component index from entity's component mappings
	INT_N transform_component_id = spawned_entity->comp_map.component_id[COMP_TRANSFORM >> 1];

	// Get pointer to newly created entity 
	comp_Transform *pTransform = &_pool_transforms.data[transform_component_id];

	// Copy transform data 
	memcpy(pTransform, &transform, sizeof(comp_Transform));
}

void TransformsUpdate(Handler *handler, float dt) {
	for(INT_N i = 0; i < _pool_transforms.count; i++) {
		comp_Transform *transform = &_pool_transforms.data[i];
		
		transform->position.y = 100 * sin(i + GetTime() * (2)) + 400;
	}
}

void PrintComponentMappings(Handler *handler, INT_N entity_id) {
	printf("----------------------------------------------------\n");
	printf("------ component mappings for entity [%04d] --------\n", entity_id);
	printf("----------------------------------------------------\n");

	Entity *entity = &handler->entities[entity_id];
	for(short i = 0; i < COMP_TYPE_COUNT; i++) {
		char id_str[32];

		if(entity->comp_map.component_id[i] > COMP_NULL)
			snprintf(id_str, sizeof(id_str), "%d", entity->comp_map.component_id[i]);
		else 
			snprintf(id_str, sizeof(id_str), "%s", COMP_NULL_ALIAS);

		printf("mapping: %s = %s\n", comp_names[i], id_str);	
	}

	printf("----------------------------------------------------\n");
}

