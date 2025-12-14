#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "raylib.h"
#include "raymath.h"
#include "handler.h"
#include "game.h"
#include "kmath.h"

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

	// Initialize spatial grid
	GridInit(&handler->grid, (Vector2){144, 144}, 64, 64);	

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

	/*
	for(int i = 0; i < 60; i++) { 
		SpawnEntity( 
			handler, (comp_Transform) { 
				.position = (Vector2){ 30 + (i * 30), 600},
				.velocity = (Vector2){ 0, 0 },
				.scale = 1, 
				.rotation = 0 
			}
		);
		
		PrintComponentMappings(handler, i);
	}
	*/
}

// Free allocated memory 
void HandlerClose(Handler *handler) {
	// Unload entities
	free(handler->entities);
	GridClose(&handler->grid);

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

	GridRenderDebugView(&handler->grid, handler);

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

			case COMP_TRANSFORM:	_pool_transforms_bind_to(mappings, i);	break;
			case COMP_SPRITE:		_pool_sprites_bind_to(mappings, i);		break;
			case COMP_SELECTABLE:	_pool_selectables_bind_to(mappings, i);	break;
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

	GridUpdate(&handler->grid, handler);
	
	for(INT_N i = 0; i < _pool_transforms.count; i++) {
		comp_Transform *transform = &_pool_transforms.data[i];

		transform->prev_position = transform->position;
		
		transform->position.y = 100 * sin(i + time * (1)) + 400;
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
	// Convert window space rectangle to game space
	rec = ScaledRecWithCamera(rec, handler->camera);
		
	// Set bit mask to components required for selection
	uint32_t mask = (COMP_TRANSFORM | COMP_SELECTABLE);

	// Iterate entities
	for(INT_N i = 0; i < handler->entity_count; i++) {
		Entity *entity = &handler->entities[i];

		// Skip selecting entities that don't have required components
		if(!(entity->components & mask)) continue;

		// Get components
		comp_Transform *transform = _pool_transforms_get(entity->comp_map.component_id[1 >> COMP_TRANSFORM]);
		comp_Selectable *selectable = _pool_selectables_get(entity->comp_map.component_id[1 >> COMP_SELECTABLE]);

		// Clear selected flag
		selectable->flags &= ~SELECTED;

		// Set selected flag on if in box
		if(CheckCollisionCircleRec(transform->position, 10, rec)) {
			selectable->flags |= SELECTED;
		}
	}
}

void GridInit(Grid *grid, Vector2 cell_size, uint16_t cols, uint16_t rows) {
	Grid new_grid = (Grid) {
		.cell_size = cell_size,
		.cols = cols,
		.rows = rows,
		.cell_count = (cols * rows),
		.cells = calloc((cols * rows), sizeof(GridCell))
	};

	*grid = new_grid;
}

void GridClose(Grid *grid) {
	free(grid->cells);
}

void GridUpdate(Grid *grid, Handler *handler) {
	// Set component mask
	uint32_t mask = (COMP_TRANSFORM);

	// Iterate entities
	for(INT_N i = 0; i < handler->entity_count; i++) {
		// Get pointer to entity
		Entity *entity = &handler->entities[i];

		// Skip iterating entities that don't have required components 
		if(!(entity->components & mask)) continue;

		// Get transform component
		comp_Transform *transform = _pool_transforms_get(entity->comp_map.component_id[1 >> COMP_TRANSFORM]);

		// Skip update if entity hasn't moved	
		if(Vector2Equals(transform->position, transform->prev_position)) continue;

		// Get transform's current position in grid 
		int16_t cell_col_curr = fabs(transform->position.x / grid->cell_size.x);
		int16_t cell_row_curr = fabs(transform->position.y / grid->cell_size.y);
		
		// Get transform's previous position in grid 
		int16_t cell_col_prev = fabs(transform->prev_position.x / grid->cell_size.x);
		int16_t cell_row_prev = fabs(transform->prev_position.y / grid->cell_size.y);

		// Skip update if entity hasn't changed cells
		if(cell_col_curr == cell_col_prev && cell_row_curr == cell_row_prev) continue;

		// Skip updates on out of bounds cells
		if(!(IsCellInBounds(cell_col_curr, cell_row_curr, grid))) continue;
		if(!(IsCellInBounds(cell_col_prev, cell_row_prev, grid))) continue;

		GridCell *cell_curr = &grid->cells[GridCoordsToId(cell_col_curr, cell_row_curr, grid)];
		GridCell *cell_prev = &grid->cells[GridCoordsToId(cell_col_prev, cell_row_prev, grid)];

		// Remove entity from previous cell
		for(int16_t j = 0; j < cell_prev->entity_count; j++) {
			if(cell_prev->entities[j] == entity->id) {
				for(uint16_t k = j; k < cell_prev->entity_count - 1; k++) {
					cell_prev->entities[k] = cell_prev->entities[k + 1];
				}

				cell_prev->entity_count--;

				break;
			}
		}

		// Add entity to current cell
		cell_curr->entities[cell_curr->entity_count++] = entity->id;
	}
}

int16_t GridCoordsToId(int16_t c, int16_t r, Grid *grid) {
	return (int16_t)(c + r * grid->cols);	
}

bool IsCellInBounds(int16_t c, int16_t r, Grid *grid) {
	if(c < 0 || r < 0) 
		return false;	

	if(c >= grid->cols - 1 || r >= grid->rows - 1)
		return false;

	return true;
}

void GridRenderDebugView(Grid *grid, Handler *handler) {
	int16_t camera_col = roundf((handler->camera->target.x / handler->camera->zoom) / grid->cell_size.x);
	int16_t camera_row = roundf((handler->camera->target.y / handler->camera->zoom) / grid->cell_size.y);

	camera_col = Clamp(camera_col, 0, grid->cols - 1);
	camera_row = Clamp(camera_row, 0, grid->rows - 1);

	int16_t frame_w = (roundf(VIRTUAL_WIDTH / handler->camera->zoom) / grid->cell_size.x) + 1;
	int16_t frame_h = (roundf(VIRTUAL_HEIGHT / handler->camera->zoom) / grid->cell_size.y) + 1;
	
	int16_t camera_col_end = camera_col + frame_w;
	int16_t camera_row_end = camera_row + frame_h;

	camera_col_end = Clamp(camera_col_end, 0, grid->cols - 1);
	camera_row_end = Clamp(camera_row_end, 0, grid->rows - 1);

	for(int16_t r = camera_row; r < camera_row_end; r++) {
		for(int16_t c = camera_col; c < camera_col_end; c++) {
			Vector2 pos = (Vector2) { .x = c * grid->cell_size.x, .y = r * grid->cell_size.y };

			DrawRectangleLines(pos.x, pos.y, grid->cell_size.x, grid->cell_size.y, GRAY);

			if(IsCellInBounds(c, r, grid)) {
				GridCell *cell = &grid->cells[GridCoordsToId(c, r, grid)];
				DrawText(TextFormat("Count: %d", cell->entity_count), pos.x, pos.y, 10, RAYWHITE);
			}
		}
	}
}

