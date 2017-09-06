/*
 *
 * Copyright (c) 2017 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "image.h"

Image::Image(Widget *parent)
	: Widget("Image", ::elm_image_add(evasObject(parent)))
{
	evas_object_size_hint_weight_set(evasObject(this), EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
}

Image::~Image()
{
}

void Image::setFile(const std::string &path)
{
	::elm_image_file_set(evasObject(this), path.c_str(), NULL);
}
