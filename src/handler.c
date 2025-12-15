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
declare_component_pool(moveables, comp_Movable);

char *comp_names[COMP_TYPE_COUNT] = {
	"transform	",
	"sprite	",
	"selectable	",
	"moveable   ",
};

void HandlerInit(Handler *handler, Camera2D *camera, float dt) {
	// Initialize component pools
	_pool_transforms_init();
	_pool_sprites_init();
	_pool_selectables_init();
	_pool_moveables_init();

	// Allocate memory for entities
	handler->entity_count = 0;
	handler->entities = calloc(ENTITY_CAP, sizeof(Entity));
	handler->comp_mappings = calloc(ENTITY_CAP, sizeof(ComponentMap));

	handler->selected_entity_count = 0;
	handler->selected_entities = calloc(ENTITY_CAP, sizeof(INT_N));

	// Set camera pointer
	handler->camera = camera;

	// Initialize spatial grid
	GridInit(&handler->grid, (Vector2){128, 128}, 128, 128);	

	for(int i = 0; i < 30; i++) { 
		SpawnEntity( 
			handler, (comp_Transform) { 
				.position = (Vector2){ (16) + (i * 32), 300},
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
	if(handler->entities) 
		free(handler->entities);

	// Unload selected entity indices
	if(handler->selected_entities)
		free(handler->selected_entities);

	// Unload spatial grid
	GridClose(&handler->grid);

	// Unload component pools
	_pool_transforms_free();
	_pool_sprites_free();
	_pool_selectables_free();
	_pool_moveables_free();
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

		DrawCircleV(transform->position, 10, ColorAlpha(RAYWHITE, 0.5f));
		DrawCircleLinesV(transform->position, 10, RAYWHITE);

		if(ent->components & COMP_SELECTABLE) {
			comp_Selectable *selectable = _pool_selectables_get(ent->comp_map.component_id[1 >> COMP_SELECTABLE]);

			if(selectable->flags & SELECTED) {
				DrawCircleLinesV(transform->position, 10, SKYBLUE);
			}
		}
	}

	//DrawCircleV(handler->command_marker_position, 5, RED);
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
			case COMP_TRANSFORM:	_pool_transforms_bind_to(mappings, i);		break;
			case COMP_SPRITE:		_pool_sprites_bind_to(mappings, i);			break;
			case COMP_SELECTABLE:	_pool_selectables_bind_to(mappings, i);		break;
			case COMP_MOVABLE:		_pool_moveables_bind_to(mappings, i);		break;
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
	INT_N id = AddEntity(handler, (COMP_TRANSFORM | COMP_SPRITE | COMP_SELECTABLE | COMP_MOVABLE));

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
		//transform->position.y = 100 * sin(i + time * (1.5f)) + 420;
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

	Grid *grid = &handler->grid;

	// Convert selection rectangle's start position to grid coordinates
	int16_t start_x = floor(rec.x / grid->cell_size.x);
	int16_t start_y = floor(rec.y / grid->cell_size.y);

	// Convert selection rectangle's end position to grid coordinates
	Vector2 end = (Vector2) { rec.x + rec.width, rec.y + rec.height };
	int16_t end_x = ceil(end.x / grid->cell_size.x);
	int16_t end_y = ceil(end.y / grid->cell_size.y);

	// Clear current selection
	// Set count to zero
	handler->selected_entity_count = 0;

	// Clear selected flag on components
	for(INT_N i = 0; i < _pool_selectables.count; i++) {
		comp_Selectable *selectable_component = &_pool_selectables.data[i];
		selectable_component->flags &= ~SELECTED;
	}

	// Set component mask
	uint32_t mask = (COMP_TRANSFORM | COMP_SELECTABLE);

	// Iterate through grid cells
	for(int16_t r = start_y; r < end_y; r++) {
		for(int16_t c = start_x; c < end_x; c++) {
			// Get pointer to cell
			GridCell *cell = &grid->cells[GridCoordsToId(c, r, grid)];

			// Iterate entities within cell
			for(INT_N i = 0; i < cell->entity_count; i++) {
				Entity *entity = &handler->entities[cell->entities[i]];	

				// Skip entities without required components for selection
				if(!(entity->components & mask)) continue;

				// Get components 
				comp_Transform *transform = _pool_transforms_get(entity->comp_map.component_id[1 >> COMP_TRANSFORM]);
				comp_Selectable *selectable = _pool_selectables_get(entity->comp_map.component_id[1 >> COMP_SELECTABLE]);
				
				// Skip entities outside of selection box
				if(!(CheckCollisionCircleRec(transform->position, 10, rec))) continue; 

				// Set selected flag on
				selectable->flags |= SELECTED;

				// Add entity's id to selection array and increment count
				handler->selected_entities[handler->selected_entity_count++] = entity->id;
			}
		}
	}
}

void ProcessCommandInput(Handler *handler, Vector2 point) {
	// Convert point from screen space to world space
	point = ScaledVec2WithCamera(point, handler->camera);
	
	handler->command_marker_position = point;

	// Set component mask
	uint32_t mask = (COMP_TRANSFORM | COMP_MOVABLE);

	// Iterate selected entities
	for(INT_N i = 0; i < handler->selected_entity_count; i++) {
		Entity *entity = &handler->entities[handler->selected_entities[i]];

		// Skip assigning command if entity does not have required componenents 
		if(!(entity->components & mask)) continue;
	}
}

// Initialize spatial grid
void GridInit(Grid *grid, Vector2 cell_size, uint16_t cols, uint16_t rows) {
	// Free allocated memory (if any exists) 
	if(grid->cells) 
		free(grid->cells);

	Grid new_grid = (Grid) {
		.cell_size = cell_size,
		.cols = cols,
		.rows = rows,
		.cell_count = (cols * rows),
		.cells = calloc((cols * rows), sizeof(GridCell))
	};

	*grid = new_grid;
}

// Free memory allocated for spatial grid
void GridClose(Grid *grid) {
	if(grid->cells) free(grid->cells);
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
		int16_t cell_col_curr =(transform->position.x / grid->cell_size.x);
		int16_t cell_row_curr =(transform->position.y / grid->cell_size.y);
		
		// Get transform's previous position in grid 
		int16_t cell_col_prev = (transform->prev_position.x / grid->cell_size.x);
		int16_t cell_row_prev = (transform->prev_position.y / grid->cell_size.y);

		// Skip update if entity hasn't changed cells
		if(cell_col_curr == cell_col_prev && cell_row_curr == cell_row_prev) continue;

		// Skip updates on out of bounds cells
		if(!(IsCellInBounds(cell_col_curr, cell_row_curr, grid))) continue;
		if(!(IsCellInBounds(cell_col_prev, cell_row_prev, grid))) continue;

		GridCell *cell_curr = &grid->cells[GridCoordsToId(cell_col_curr, cell_row_curr, grid)];
		GridCell *cell_prev = &grid->cells[GridCoordsToId(cell_col_prev, cell_row_prev, grid)];

		// Remove entity from previous cell
		// Search for entity
		for(INT_N j = cell_prev->entity_count; j >= 0; j--) {
			INT_N to_remove = cell_prev->entities[j];
			
			if(to_remove == entity->id) {
				// Compact array
				for(uint16_t k = j; k < cell_prev->entity_count - 1; k++) 
					cell_prev->entities[k] = cell_prev->entities[k + 1];

				// Decrement count
				if(cell_prev->entity_count > 0)
					cell_prev->entity_count--;

				break;
			}
		}

		// Don't add if cell is full 
		if(cell_curr->entity_count - 1 > MAX_ENTITIES_PER_CELL) 
			continue;	

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

	if(c > grid->cols - 1 || r > grid->rows - 1)
		return false;

	return true;
}

void GridRenderDebugView(Grid *grid, Handler *handler) {
	float z = handler->camera->zoom;

	int16_t frame_w = ((VIRTUAL_WIDTH) / (grid->cell_size.x)) / z;
	int16_t frame_h = ((VIRTUAL_HEIGHT) / (grid->cell_size.y) / z);
	
	Vector2 cam_pos = GetScreenToWorld2D(Vector2Zero(), *handler->camera);
	Vector2 cam_end = GetScreenToWorld2D((Vector2){GetScreenWidth(), GetScreenHeight()}, *handler->camera);

	int16_t camera_col = cam_pos.x / grid->cell_size.x;
	int16_t camera_row = cam_pos.y / grid->cell_size.y;

	int16_t camera_col_end = cam_end.x / grid->cell_size.x;
	int16_t camera_row_end = cam_end.y / grid->cell_size.y;

	camera_col_end = Clamp(camera_col_end, frame_w, grid->cols);
	camera_row_end = Clamp(camera_row_end, frame_h, grid->rows);

	camera_col = Clamp(camera_col, 0, grid->cols - frame_w);
	camera_row = Clamp(camera_row, 0, grid->rows - frame_h);

	for(int16_t r = camera_row; r < camera_row_end; r++) {
		for(int16_t c = camera_col; c < camera_col_end; c++) {
			Vector2 pos = (Vector2) { .x = c * grid->cell_size.x, .y = r * grid->cell_size.y };

			Color color = DARKGRAY;

			GridCell *cell = &grid->cells[GridCoordsToId(c, r, grid)];
			if(cell->entity_count > 0) color = RAYWHITE;

			Rectangle rec = (Rectangle) {
				.x = pos.x,
				.y = pos.y,
				.width = grid->cell_size.x,
				.height = grid->cell_size.y
			};

			if(cell->flags & 0x01)
				color = SKYBLUE;

			DrawRectangleLinesEx(rec, 1.5f, color);
			DrawText(TextFormat("%d", cell->entity_count), pos.x + 4, pos.y + 4, 10, color);
		}
	}
}

