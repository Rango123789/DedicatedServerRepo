// Fill out your copyright notice in the Description page of Project Settings.


#include "Types/LobbyPlayerInfo.h"

/*Universal trick when you're about to Remove or Add Item
+when you add an item (it add to the end only), hence use MarkItemDirty()
+when you remove an item (either you swap it or else, at leat one final change its index abnormally), hence use MarkArrayDirty()

+When remove, search for "PreReplicatedRemove" in FFastSerializer.h
, there is 2 versions of FFastArraySerializerItem and FFastArraySerializer
, but the preffered one is FFastArraySerializerItem::____(*THIS)
+When add, search for "PostReplicatedAdd" in FFastSerializer.h
, there is 2 versions of FFastArraySerializerItem and FFastArraySerializer
, but the preferred one is FFastArraySerializerItem::____(*THIS)

+Warning: do not call those functions on "passed-in params" that may not any effect on the copy Items[i]
*/
void FLobbyPlayerInfoArray::AddPlayerInfo(FLobbyPlayerInfo InLobbyPlayerInfo)
{
	//int32 IndexOfAddedItem = Items.Add(InLobbyPlayerInfo);		
	int32 IndexOfAddedItem = Items.AddUnique(InLobbyPlayerInfo); //you must define operator== for it		
	//by doing this you only try to effect the passed-in params - NOT the element added to the array (that is a copy of passed-in param)., also the .PostReplicatedAdd modify the item itself, hence you can't use "const T&" for param:
		//InLobbyPlayerInfo.PostReplicatedAdd(*this);
	//this is the right way to do it:
	Items[IndexOfAddedItem].PostReplicatedAdd(*this);

	//the same logic here, you must work on the added item, not the passed-in param:
	MarkItemDirty(Items[IndexOfAddedItem]); 
}

void FLobbyPlayerInfoArray::RemovePlayerInfo(const FString& InUsername)
{
	/*MY IDEA: you may need to define "operator==" as well, not good!
	for (auto& Item : Items)
	{
		if (Item.Username == InUsername)
		{
			//this will shift all the elements on the right of the element one unit of index to the left (it could be that the replication only on the index + the removed element only (I'm not sure about this yet), but technically it is not best for local performance even if it doesn't replicate the whole array right? yeah)
				//Items.Remove(Item);

			//this remove all instances that make "operator==(InItem , Items::__ )" return true, meaning we may need to define one lol
				//Items.RemoveSwap(Item);

			//you can't use this version because you don't know Index in range-based loop:
				//Items.RemoveAtSwap(IndexOfItemTobeRemoved)
			
			//this only remove the first found one, first it swap with the final element and then remove the final element! but it still need to loop into the array again lol - hence stephen idea is better for performance!
			//also you may need to define "operator==" as well, not good!
			Items.RemoveSingleSwap(Item);
			
			break;
		}
	}*/
	//STEPHEN IDEA: so that i don't loop through Items twice (RemoveAt__() <=> know the index to be removed already, where Remove__() didn't know)
	for (int32 PlayerIndex = 0; PlayerIndex < Items.Num(); PlayerIndex++)
	{
		if (Items[PlayerIndex].Username == InUsername)
		{
			//apparently you can also do F__Array::PreReplicatedRemove(i), but this is NOT preferred I believe:
				//PreReplicatedRemove({i}, Items.Num() - 1);
			Items[PlayerIndex].PreReplicatedRemove(*this);
			Items.RemoveAtSwap(PlayerIndex);
			
			MarkArrayDirty(); //this one require no param nor item of course!
			break;
		}
	}	
}
