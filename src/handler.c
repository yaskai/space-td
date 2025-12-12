#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "raylib.h"
#include "raymath.h"
#include "handler.h"
#include "game.h"

// Declare component pools
declare_component_pool(transforms, comp_Transform);
declare_component_pool(sprites, comp_Sprite);
declare_component_pool(selectables, comp_Selectable);

char *comp_names[COMP_TYPE_COUNT] = {
	"transform	",
	"sprite	",
	"selectable	"
};

void HandlerInit(Handler *handler, Camera2D *camera, float dt) {
	// Initialize component pools
	_pool_transforms_init();
	_pool_sprites_init();
	_pool_selectables_init();

	// Allocate memory for entities
	handler->entity_count = 0;
	handler->entities = calloc(ENTITY_CAP, sizeof(Entity));
	handler->comp_mappings = calloc(ENTITY_CAP, sizeof(ComponentMap));

	// Set camera pointer
	handler->camera = camera;

	for(int i = 0; i < 60; i++) { 
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

	for(int i = 0; i < 60; i++) { 
		SpawnEntity( 
			handler, (comp_Transform) { 
				.position = (Vector2){ 30 + (i * 30), 800},
				.velocity = (Vector2){ 0, 0 },
				.scale = 1, 
				.rotation = 0 
			}
		);
		
		PrintComponentMappings(handler, i);
	}
}

// Free allocated memory 
void HandlerClose(Handler *handler) {
	// Unload entities
	free(handler->entities);

	// Unload component pools
	_pool_transforms_free();
	_pool_sprites_free();
	_pool_selectables_free();
}

void HandlerUpdate(Handler *handler, float dt) {
	TransformsUpdate(handler, dt);
}

void HandlerDraw(Handler *handler) {
	//DrawText(TextFormat("entity_count: %d", handler->entity_count), 100, 100, 30, RAYWHITE);

	for(INT_N i = 0; i < handler->entity_count; i++) {
		Entity *ent = &handler->entities[i];

		uint32_t mask = (COMP_TRANSFORM | COMP_SPRITE);
		if(!(ent->components & mask)) continue;

		comp_Transform *transform = _pool_transforms_get(ent->comp_map.component_id[1 >> COMP_TRANSFORM]);
		comp_Sprite *sprite = _pool_sprites_get(ent->comp_map.component_id[1 >> COMP_SPRITE]);

		DrawCircleLinesV(transform->position, 10, RAYWHITE);

		if(ent->components & COMP_SELECTABLE) {
			comp_Selectable *selectable = _pool_selectables_get(ent->comp_map.component_id[1 >> COMP_SELECTABLE]);

			if(selectable->flags & SELECTED) {
				DrawCircleLinesV(transform->position, 10, SKYBLUE);
			}
		}
	}
}

INT_N AddEntity(Handler *handler, uint32_t components) {
	// Initialize component mappings for new entity
	// By default, all entries map to nothing
	INT_N mappings[COMP_TYPE_COUNT] = { 0 };
	memset(mappings, COMP_NULL, sizeof(mappings));

	// Create new components and register their IDs to the mapping  
	for(uint32_t i = 0; i < COMP_TYPE_COUNT; i++) {
		uint32_t mask = (1 << i);

		if(!(components & mask)) continue; 

		switch(mask) {
			printf("adding %s component...\n", comp_names[i]);

			case COMP_TRANSFORM: {
				// Create empty transform
				comp_Transform new_transform = (comp_Transform) { 0 };

				// Add transform to pool 
				INT_N comp_id = _pool_transforms_add(new_transform);

				// Add transform's id to entity component mappings 
				mappings[i] = comp_id;

			} break;

			case COMP_SPRITE: {
				// Create empty sprite
				comp_Sprite new_sprite = (comp_Sprite) { 0 };

				// Add sprite to pool 
				INT_N comp_id = _pool_sprites_add(new_sprite);

				// Add sprite's id to entity component mappings 
				mappings[i] = comp_id;

			} break;

			case COMP_SELECTABLE: {
				// Create empty selectable
				comp_Selectable new_selectable = (comp_Selectable) { 0 };

				// Add selectable to pool 
				INT_N comp_id = _pool_selectables_add(new_selectable);

				// Add selectable's id to entity component mappings 
				mappings[i] = comp_id;

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
	INT_N id = AddEntity(handler, (COMP_TRANSFORM | COMP_SPRITE | COMP_SELECTABLE));

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
	float time = GetTime(); 
	for(INT_N i = 0; i < _pool_transforms.count; i++) {
		comp_Transform *transform = &_pool_transforms.data[i];
		
		//transform->position.y = 100 * sin(i + time * (1)) + 400;
		//transform->position.y += sin(i + time * (1));
	}
}

void PrintComponentMappings(Handler *handler, INT_N entity_id) {
	printf("____________________________________________________\n");
	printf("______ component mappings for entity [%04d] ________\n", entity_id);
	printf("____________________________________________________\n");

	Entity *entity = &handler->entities[entity_id];
	for(short i = 0; i < COMP_TYPE_COUNT; i++) {
		char id_str[32];

		if(entity->comp_map.component_id[i] > COMP_NULL)
			snprintf(id_str, sizeof(id_str), "%d", entity->comp_map.component_id[i]);
		else 
			snprintf(id_str, sizeof(id_str), "%s", COMP_NULL_ALIAS);

		printf("| mapping: %s = %s\n", comp_names[i], id_str);	
	}

	printf("____________________________________________________\n");
}

void CheckSelectedUnits(Handler *handler, Rectangle rec) {
	uint32_t mask = (COMP_TRANSFORM | COMP_SELECTABLE);

	for(INT_N i = 0; i < handler->entity_count; i++) {
		Entity *entity = &handler->entities[i];

		if(!(entity->components & mask)) continue;

		comp_Selectable *selectable = _pool_selectables_get(entity->comp_map.component_id[1 >> COMP_SELECTABLE]);
		comp_Transform *transform = _pool_transforms_get(entity->comp_map.component_id[1 >> COMP_TRANSFORM]);

		selectable->flags &= ~SELECTED;

		if(CheckCollisionCircleRec(transform->position, 10, rec)) {
			selectable->flags |= SELECTED;
		}

	}
}

