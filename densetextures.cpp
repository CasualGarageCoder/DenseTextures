#include "densetextures.h"


bool HiResTexture::_set(const StringName &p_name, const Variant &p_value) {
	if (p_name == "image") {
		if (!create_from_image(p_value)) {
			ERR_FAIL_V_MSG(false, "Can't create from image while setting the property 'image'");
		}
	} else if (p_name == "size") {
		Size2 s = p_value;
		bool recreate = (w != s.width || h != s.height);
		w = s.width;
		h = s.height;
		if (recreate) {
			create(w, h);
		}
	} else if (p_name == "_data") {
		_set_data(p_value);
	} else {
		return false;
	}

	return true;
}

bool HiResTexture::_get(const StringName &p_name, Variant &r_ret) const {
	if (p_name == "image_data") {
	} else if (p_name == "image") {
		r_ret = get_data();
	} else if (p_name == "size") {
		r_ret = Size2(w, h);
	} else {
		return false;
	}

	return true;
}

void HiResTexture::_get_property_list(List<PropertyInfo> *p_list) const {
	p_list->push_back(PropertyInfo(Variant::VECTOR2, "size", PROPERTY_HINT_NONE, ""));
}

void HiResTexture::set_data(const Ref<Image> &p_image) {
	ERR_FAIL_COND_MSG(p_image.is_null(), "Invalid image");
	bool size_cond = p_image->get_width()!=w||p_image->get_height()!=h;
	ERR_FAIL_COND_MSG(size_cond, "Invalid image size");

	VisualServer::get_singleton()->texture_set_data(texture, p_image);

	_change_notify();
	emit_changed();
}

Ref<Image> HiResTexture::get_data() const {
	return VisualServer::get_singleton()->texture_get_data(texture);
}

int HiResTexture::get_width() const {
	return w;
}

int HiResTexture::get_height() const {
	return h;
}

RID HiResTexture::get_rid() const {
	return texture;
}

bool HiResTexture::has_alpha() const {
	return true;
}

void HiResTexture::create(int p_width, int p_height) {
	ERR_FAIL_COND_MSG(p_width==0 || p_height == 0, "Can't create a texture with 0 in size coordinates"); 
	VisualServer::get_singleton()->texture_allocate(texture, p_width, p_height, 0, Image::FORMAT_RGBAF, VS::TEXTURE_TYPE_2D, 0);
	w = p_width;
	h = p_height;
	Ref<Image> img = memnew(Image(w, h, false, Image::FORMAT_RGBAF));
	VisualServer::get_singleton()->texture_set_data(texture, img);
	_change_notify();
	emit_changed();
}

bool HiResTexture::create_from_image(const Ref<Image> &img) {
	ERR_FAIL_COND_V_MSG(img.is_null(), false, "(null) reference to image");
	ERR_FAIL_COND_V_MSG(img->empty(), false, "Invalid image : empty");
	ERR_FAIL_COND_V_MSG(img->get_format()!=Image::FORMAT_RGBAF, false, "Invalid image : Not HDR");

	w = img->get_width();
	h = img->get_height();

	VisualServer::get_singleton()->texture_allocate(texture, w, h, 0, Image::FORMAT_RGBAF, VS::TEXTURE_TYPE_2D, 0); // Default Flg
	VisualServer::get_singleton()->texture_set_data(texture, img);
	_change_notify();
	emit_changed();
	return true;
}

void HiResTexture::_set_data(Dictionary p_data) {
	Ref<Image> img = p_data["image"];
	ERR_FAIL_COND(!img.is_valid());

	create_from_image(img);
};

void HiResTexture::_bind_methods() {
	ClassDB::bind_method(D_METHOD("create", "width", "height"), &HiResTexture::create);
	ClassDB::bind_method(D_METHOD("set_data", "image"), &HiResTexture::set_data);
}

HiResTexture::HiResTexture() {
	w = h = 0;
	texture = RID_PRIME(VisualServer::get_singleton()->texture_create());
}

HiResTexture::~HiResTexture() {
	VisualServer::get_singleton()->free(texture);
}

DenseGradientTexture::DenseGradientTexture() {
	width = 2048;
	texture_reset = true;
	texture = RID_PRIME(VS::get_singleton()->texture_create());
	_update();
}

DenseGradientTexture::~DenseGradientTexture() {
	VS::get_singleton()->free(texture);
}

void DenseGradientTexture::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_gradient", "gradient"), &DenseGradientTexture::set_gradient);
	ClassDB::bind_method(D_METHOD("get_gradient"), &DenseGradientTexture::get_gradient);

	ClassDB::bind_method(D_METHOD("set_width", "width"), &DenseGradientTexture::set_width);

	ClassDB::bind_method(D_METHOD("_update"), &DenseGradientTexture::_update);

	ClassDB::bind_method(D_METHOD("update_gradient_only", "gradient"), &DenseGradientTexture::update_gradient_only);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "gradient", PROPERTY_HINT_RESOURCE_TYPE, "Gradient"), "set_gradient", "get_gradient");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "width", PROPERTY_HINT_RANGE, "16,8192"), "set_width", "get_width");
}

void DenseGradientTexture::set_gradient(Ref<Gradient> p_gradient) {
	if (p_gradient == gradient) {
		return;
	}
	if (gradient.is_valid()) {
		gradient->disconnect(CoreStringNames::get_singleton()->changed, this, "_update");
	}
	gradient = p_gradient;
	if (gradient.is_valid()) {
		gradient->connect(CoreStringNames::get_singleton()->changed, this, "_update");
	}
	_update();
	emit_changed();
}

Ref<Gradient> DenseGradientTexture::get_gradient() const {
	return gradient;
}

void DenseGradientTexture::_update() {
	if (gradient.is_null()) {
		return;
	}

	Ref<Image> image;

	if (texture_reset) {
		image.instance();
		image->create(width, 1, false, Image::FORMAT_RGBAF);
	} else {
		image = get_data();
	}

	Gradient &g = **gradient;
	image->lock();
	for (int x = 0; x < width; x++) {
		float ofs = float(x) / (width - 1);
		image->set_pixel(x, 0, g.get_color_at_offset(ofs));
	}
	image->unlock();

	if (texture_reset) {
		VS::get_singleton()->texture_allocate(texture, width, 1, 0, image->get_format(), VS::TEXTURE_TYPE_2D, VS::TEXTURE_FLAG_FILTER);
	}
	VS::get_singleton()->texture_set_data(texture, image);
	texture_reset = false;
	emit_changed();
}

void DenseGradientTexture::update_gradient_only(Ref<Gradient> p_gradient) {
	texture_reset = false;
	set_gradient(p_gradient);
}

void DenseGradientTexture::set_width(int p_width) {
	texture_reset = true;
	if(p_width == width) {
		return;
	}
	width = p_width;
	_update();
	emit_changed();
}
int DenseGradientTexture::get_width() const {
	return width;
}

Ref<Image> DenseGradientTexture::get_data() const {
	return VisualServer::get_singleton()->texture_get_data(texture);
}
