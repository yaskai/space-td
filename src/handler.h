#include <stddef.h>
#include <stdint.h>
#include "raylib.h"

#ifndef HANDLER_H_
#define HANDLER_H_

// Type used for indexing/count of entities and components, easy to change if needed
#define INT_N int16_t

#define ENTITY_CAP 	1024
#define COMP_CAP	1024

// How many component types there are
#define COMP_TYPE_COUNT 	32

// No component
#define COMP_NULL			-1
#define COMP_NULL_ALIAS		"comp_null"

// Maximum log message size
#define MESSAGE_CAP			1024

enum COMP_HEXVALS : uint32_t {
		COMP_TRANSFORM		= 0x00000001,
		COMP_SPRITE			= 0x00000002,
		COMP_SELECTABLE		= 0x00000004,
		COMP_MOVEABLE		= 0x00000008,
		EMPTY_04		 	= 0x00000010,
		EMPTY_05		 	= 0x00000020,
		EMPTY_06		 	= 0x00000040,
		EMPTY_07		 	= 0x00000080,
		EMPTY_08		 	= 0x00000100,
		EMPTY_09		 	= 0x00000200,
		EMPTY_10		 	= 0x00000400,
		EMPTY_11		 	= 0x00000800,
		EMPTY_12 			= 0x00001000,
		EMPTY_13 			= 0x00002000,
		EMPTY_14 			= 0x00004000,
		EMPTY_15 			= 0x00008000,
		EMPTY_16 			= 0x00010000,
		EMPTY_17 			= 0x00020000,
		EMPTY_18 			= 0x00040000,
		EMPTY_19 			= 0x00080000,
		EMPTY_20 			= 0x00100000,
		EMPTY_21 			= 0x00200000,
		EMPTY_22 			= 0x00400000,
		EMPTY_23 			= 0x00800000,
		EMPTY_24 			= 0x01000000,
		EMPTY_25 			= 0x02000000,
		EMPTY_26 			= 0x04000000,
		EMPTY_27 			= 0x08000000,
		EMPTY_28 			= 0x10000000,
		EMPTY_29 			= 0x20000000,
		EMPTY_30 			= 0x40000000,
		EMPTY_31 			= 0x80000000
};

// Component mapping struct
// Has array containining indices of components. 
typedef struct {
	// *
	// Component types can be found with a 1 shifting by index 
	// Useful for referring to specific component types
	//
	// eg.   
	// component_id[0] = 99  
	// translates to:
	// component of type 1 >> 0 has index 99 in it's pool  
	// so, 1 >> 0 = 0x01 meaning:  
	// transform component for this mapping is located at transforms[99]
	INT_N component_id[COMP_TYPE_COUNT];
	// *
} ComponentMap;

// Base entity struct 
typedef struct {
	ComponentMap comp_map;
	uint32_t components;
	INT_N id;

	uint8_t flags;

} Entity;

// ----------------------------------------
// 			Component Definitions 
// ----------------------------------------
//
// Transform component 
typedef struct {
	Vector2 position;
	Vector2 velocity;
	Vector2 scale;
	Vector2 prev_position;

	float rotation;

} comp_Transform;

// Sprite component
typedef struct {
	uint16_t frame;
	uint16_t sprite_id; 

	float rotation;

	uint8_t flags;

} comp_Sprite;

// Selectable component
#define SELECTED	0x01
typedef struct {
	uint8_t flags;

} comp_Selectable;

// Moveable component
#define MOVING			0x01	
#define REACHED_TARGET	0x02
typedef struct {
	Vector2 target;

	uint8_t flags;
	
} comp_Movable;

// ----------------------------------------

// ----------------------------------------
// 			Spatial Partitioning 
// ----------------------------------------
#define MAX_ENTITIES_PER_CELL 128

typedef struct {
	INT_N entities[MAX_ENTITIES_PER_CELL];
	INT_N entity_count;

	uint8_t flags;

} GridCell;

typedef struct {
	GridCell *cells;

	Vector2 cell_size;

	uint16_t cols;
	uint16_t rows;
	uint16_t cell_count;

} Grid;
// ----------------------------------------

#define COMMAND_CAP	32
typedef struct {
	Vector2 target;

	INT_N unit;

} MoveCommand;

typedef struct {
	INT_N unit;
	INT_N target;

} AttackCommand;

// Handler struct 
// Stores all entity and component data
// Data is modified with 'ComponentUpdate()' functions
typedef struct {
	// Entity array
	Entity *entities;

	// Selected entity indices array
	INT_N *selected_entities;
	
	// Spatial grid struct
	Grid grid;

	// Component mapping array	
	ComponentMap *comp_mappings;

	// Pointer to camera struct
	Camera2D *camera;

	// Movement command array
	MoveCommand *move_commands;
	uint16_t *move_commands_free;

	Vector2 command_marker_position;

	uint16_t move_command_count, move_command_free_count;

	// Count and capacity for entity array:
	INT_N entity_count; 
	INT_N entity_capacity;

	// How many entities are currently in selection
	INT_N selected_entity_count;
	

} Handler;

// ----------------------------------------
// 		    Component Pool Macros 
// ----------------------------------------
#define define_component_pool(_name, _type)	\
typedef struct {	\
	_type *data;	\
	INT_N count;	\
	INT_N capacity;	\
} _name;

#define declare_component_pool(_name, _type)	\
	define_component_pool(_name, _type)	\
	_name _pool_##_name = (_name) {	\
		.data = NULL,	\
		.count = 0,	\
		.capacity = COMP_CAP,	\
	};	\
	void _pool_##_name##_init() { \
		_pool_##_name.data = calloc(COMP_CAP, sizeof(_type)); \
	} \
	INT_N _pool_##_name##_add(_type thing) { \
		_pool_##_name.data[_pool_##_name.count] = thing;	\
		return _pool_##_name.count++; \
	}	\
	_type* _pool_##_name##_get(INT_N id) { \
		return &_pool_##_name.data[id]; \
	} \
	void _pool_##_name##_free() { \
		if(_pool_##_name.data) free(_pool_##_name.data);	\
	}	\
	void _pool_##_name##_bind_to(INT_N *mappings, uint32_t i) {	\
		_type component = (_type) { 0 }; \
		INT_N comp_id = _pool_##_name##_add(component); \
		mappings[i] = comp_id;	\
	}
// ----------------------------------------

// Initalize handler:
// Allocate memory for entity and component arrays,
// set pointers, defaults, etc.
void HandlerInit(Handler *handler, Camera2D *camera, float dt);

void HandlerClose(Handler *handler);

// Update all systems
void HandlerUpdate(Handler *handler, float dt);

// Draw entities
// *NOTE:
// will be moved later to 'render.h',
// sprite draw requests will be sent to renderer.
// Renderer will process requests then draw them to buffer
void HandlerDraw(Handler *handler);

// Create a new entity,
// insert entity and it's components to respective arrays
INT_N AddEntity(Handler *handler, uint32_t components);

void SpawnEntity(Handler *handler, comp_Transform transform);

INT_N TransformAdd(Handler *handler, comp_Transform comp_transform);
void TransformsUpdate(Handler *handler, float dt);

void SpritesUpdate(Handler *handler, float dt);

void PrintComponentMappings(Handler *handler, INT_N entity_id);
void HandlerLogMessage(Handler *handler, char message[]);

void CheckSelectedUnits(Handler *handler, Rectangle rec);
void ProcessCommandInput(Handler *handler, Vector2 point);

void GridInit(Grid *grid, Vector2 cell_size, uint16_t cols, uint16_t rows);
void GridClose(Grid *grid);
void GridUpdate(Grid *grid, Handler *handler);

int16_t GridCoordsToId(int16_t c, int16_t r, Grid *grid);
bool IsCellInBounds(int16_t c, int16_t r, Grid *grid);

void GridRenderDebugView(Grid *grid, Handler *handler);

void MoveSystemUpdate(Handler *handler, float dt);

void DebugDrawCommandInfo(Handler *handler);

#endif

