internal ui_id_t UI_CreateUIID()
{
    local_persist ui_id_t nextID = 0;
    return ++nextID;
}

/*
internal void UI_DrawSpriteButton(ui_t ui, texture_t texture, v2 position)
{

}
*/
