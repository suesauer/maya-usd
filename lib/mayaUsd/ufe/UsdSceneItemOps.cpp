//
// Copyright 2019 Autodesk
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
#include "UsdSceneItemOps.h"

#include <mayaUsd/ufe/UsdUndoDeleteCommand.h>
#include <mayaUsd/ufe/UsdUndoDuplicateCommand.h>
#include <mayaUsd/ufe/UsdUndoRenameCommand.h>
#include <mayaUsd/ufe/Utils.h>

MAYAUSD_NS_DEF {
namespace ufe {

UsdSceneItemOps::UsdSceneItemOps(const UsdSceneItem::Ptr& item)
	: Ufe::SceneItemOps(), fItem(item), fPrim(item->prim())
{
}

UsdSceneItemOps::~UsdSceneItemOps()
{
}

/*static*/
UsdSceneItemOps::Ptr UsdSceneItemOps::create(const UsdSceneItem::Ptr& item)
{
	return std::make_shared<UsdSceneItemOps>(item);
}

void UsdSceneItemOps::setItem(const UsdSceneItem::Ptr& item)
{
	fPrim = item->prim();
	fItem = item;
}

const Ufe::Path& UsdSceneItemOps::path() const
{
	return fItem->path();
}

//------------------------------------------------------------------------------
// Ufe::SceneItemOps overrides
//------------------------------------------------------------------------------

Ufe::SceneItem::Ptr UsdSceneItemOps::sceneItem() const
{
	return fItem;
}

Ufe::UndoableCommand::Ptr UsdSceneItemOps::deleteItemCmd()
{
	auto deleteCmd = UsdUndoDeleteCommand::create(fPrim);
	deleteCmd->execute();
	return deleteCmd;
}

bool UsdSceneItemOps::deleteItem()
{
	return fPrim.SetActive(false);
}

Ufe::Duplicate UsdSceneItemOps::duplicateItemCmd()
{
	auto duplicateCmd = UsdUndoDuplicateCommand::create(fPrim, fItem->path());
	duplicateCmd->execute();
	auto item = createSiblingSceneItem(path(), duplicateCmd->usdDstPath().GetElementString());
	return Ufe::Duplicate(item, duplicateCmd);
}

Ufe::SceneItem::Ptr UsdSceneItemOps::duplicateItem()
{
	SdfPath usdDstPath;
	SdfLayerHandle layer;
	UsdUndoDuplicateCommand::primInfo(fPrim, usdDstPath, layer);
	bool status = UsdUndoDuplicateCommand::duplicate(layer, fPrim.GetPath(), usdDstPath);

	// The duplicate is a sibling of the source.
	if (status)
		return createSiblingSceneItem(path(), usdDstPath.GetElementString());

	return nullptr;
}

Ufe::SceneItem::Ptr UsdSceneItemOps::renameItem(const Ufe::PathComponent& newName)
{
	auto renameCmd = UsdUndoRenameCommand::create(fItem, newName);
	renameCmd->execute();
	return renameCmd->renamedItem();
}

Ufe::Rename UsdSceneItemOps::renameItemCmd(const Ufe::PathComponent& newName)
{
	auto renameCmd = UsdUndoRenameCommand::create(fItem, newName);
	renameCmd->execute();
	return Ufe::Rename(renameCmd->renamedItem(), renameCmd);
}

} // namespace ufe
} // namespace MayaUsd
