internal inline void Entity_ZeroMovement(Entity *e)
{
    e->position = {0, 0};
    e->velocity = {0, 0};
    e->acceleration = {0, 0};
}

internal inline v2 Entity_BoundingBox(Entity *e)
{
    return {(f32)e->image.width * DRAW_SCALE, (f32)e->image.height * DRAW_SCALE};
}

internal void Entity_Move(Tilemap *map, Entity *e)
{
    const i32 subStep = 8;
    const v2 velocity = e->velocity / subStep;

    for (i32 i = 0; i < subStep; i++)
    {
        v2 oldPosition = e->position;

        e->position.y += velocity.y;
        if (Tilemap_DetectCollision(map, e))
        {
            e->position = oldPosition;
            e->velocity.y = 0;
        }

        oldPosition = e->position;
        e->position.x += velocity.x;
        if (Tilemap_DetectCollision(map, e))
        {
            e->position = oldPosition;
            e->velocity.x = 0;
        }
    }

    e->position = V2Clamp(e->position, {0, 0}, Tilemap_BoundingBox(map) - Entity_BoundingBox(e));
}
