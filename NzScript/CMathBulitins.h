void LoadCMath(ScriptContext& ctx) {
	ctx.InternalConstants["Pi"] = std::atan(1.0) * 4;
	ctx.InternalConstants["E"] = std::exp(1.0);
	ctx.InternalConstants["NaN"] = 1.0 / 0.0 * 0.0;
	ctx.InternalFunctions["abs"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 1)
			throw std::exception("Usage: abs(x).");
		auto v = vars[0];
		if (v.Type == Variant::DataType::Double) {
			return abs(v.Double);
		}
		if (v.Type == Variant::DataType::Float) {
			return abs(v.Float);
		}
		if (v.Type == Variant::DataType::Int) {
			return abs(v.Int);
		}
		if (v.Type == Variant::DataType::Long) {
			return abs(v.Long);
		}
		throw std::exception("Input must be a number.");
	};
	ctx.InternalFunctions["acos"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 1)
			throw std::exception("Usage: acos(x).");
		auto v = vars[0];
		if (v.Type == Variant::DataType::Double) {
			return acos(v.Double);
		}
		if (v.Type == Variant::DataType::Float) {
			return acos(v.Float);
		}
		if (v.Type == Variant::DataType::Int) {
			return acos(v.Int);
		}
		if (v.Type == Variant::DataType::Long) {
			return acos(v.Long);
		}
		throw std::exception("Input must be a number.");
	};
	ctx.InternalFunctions["asin"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 1)
			throw std::exception("Usage: asin(x).");
		auto v = vars[0];
		if (v.Type == Variant::DataType::Double) {
			return asin(v.Double);
		}
		if (v.Type == Variant::DataType::Float) {
			return asin(v.Float);
		}
		if (v.Type == Variant::DataType::Int) {
			return asin(v.Int);
		}
		if (v.Type == Variant::DataType::Long) {
			return asin(v.Long);
		}
		throw std::exception("Input must be a number.");
	};
	ctx.InternalFunctions["atan"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 1)
			throw std::exception("Usage: atan(x).");
		auto v = vars[0];
		if (v.Type == Variant::DataType::Double) {
			return atan(v.Double);
		}
		if (v.Type == Variant::DataType::Float) {
			return atan(v.Float);
		}
		if (v.Type == Variant::DataType::Int) {
			return atan(v.Int);
		}
		if (v.Type == Variant::DataType::Long) {
			return atan(v.Long);
		}
		throw std::exception("Input must be a number.");
	};
	ctx.InternalFunctions["atan2"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 2)
			throw std::exception("Usage: atan2(a,b).");
		auto lft = vars[0];
		auto rht = vars[1];
		if (lft.Type == Variant::DataType::Double || rht.Type == Variant::DataType::Double) {
			return atan2(script_cast<double>(lft), script_cast<double>(rht));
		}
		if (lft.Type == Variant::DataType::Float || rht.Type == Variant::DataType::Float) {
			return atan2(script_cast<float>(lft), script_cast<float>(rht));
		}
		if (lft.Type == Variant::DataType::Long || rht.Type == Variant::DataType::Long) {
			return atan2(script_cast<long long>(lft), script_cast<long long>(rht));
		}
		if (lft.Type == Variant::DataType::Int || rht.Type == Variant::DataType::Int) {
			return atan2(script_cast<int>(lft), script_cast<int>(rht));
		}

		return {};
	};
	ctx.InternalFunctions["ceil"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 1)
			throw std::exception("Usage: ceil(x).");
		auto v = vars[0];
		if (v.Type == Variant::DataType::Double) {
			return ceil(v.Double);
		}
		if (v.Type == Variant::DataType::Float) {
			return ceil(v.Float);
		}
		if (v.Type == Variant::DataType::Int) {
			return ceil(v.Int);
		}
		if (v.Type == Variant::DataType::Long) {
			return ceil(v.Long);
		}
		throw std::exception("Input must be a number.");
	};
	ctx.InternalFunctions["cos"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 1)
			throw std::exception("Usage: cos(x).");
		auto v = vars[0];
		if (v.Type == Variant::DataType::Double) {
			return cos(v.Double);
		}
		if (v.Type == Variant::DataType::Float) {
			return cos(v.Float);
		}
		if (v.Type == Variant::DataType::Int) {
			return cos(v.Int);
		}
		if (v.Type == Variant::DataType::Long) {
			return cos(v.Long);
		}
		throw std::exception("Input must be a number.");
	};
	ctx.InternalFunctions["cosh"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 1)
			throw std::exception("Usage: cosh(x).");
		auto v = vars[0];
		if (v.Type == Variant::DataType::Double) {
			return cosh(v.Double);
		}
		if (v.Type == Variant::DataType::Float) {
			return cosh(v.Float);
		}
		if (v.Type == Variant::DataType::Int) {
			return cosh(v.Int);
		}
		if (v.Type == Variant::DataType::Long) {
			return cosh(v.Long);
		}
		throw std::exception("Input must be a number.");
	};
	ctx.InternalFunctions["exp"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 1)
			throw std::exception("Usage: exp(x).");
		auto v = vars[0];
		if (v.Type == Variant::DataType::Double) {
			return exp(v.Double);
		}
		if (v.Type == Variant::DataType::Float) {
			return exp(v.Float);
		}
		if (v.Type == Variant::DataType::Int) {
			return exp(v.Int);
		}
		if (v.Type == Variant::DataType::Long) {
			return exp(v.Long);
		}
		throw std::exception("Input must be a number.");
	};
	ctx.InternalFunctions["fabs"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 1)
			throw std::exception("Usage: fabs(x).");
		auto v = vars[0];
		if (v.Type == Variant::DataType::Double) {
			return fabs(v.Double);
		}
		if (v.Type == Variant::DataType::Float) {
			return fabs(v.Float);
		}
		if (v.Type == Variant::DataType::Int) {
			return fabs(v.Int);
		}
		if (v.Type == Variant::DataType::Long) {
			return fabs(v.Long);
		}
		throw std::exception("Input must be a number.");
	};
	ctx.InternalFunctions["floor"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 1)
			throw std::exception("Usage: floor(x).");
		auto v = vars[0];
		if (v.Type == Variant::DataType::Double) {
			return floor(v.Double);
		}
		if (v.Type == Variant::DataType::Float) {
			return floor(v.Float);
		}
		if (v.Type == Variant::DataType::Int) {
			return floor(v.Int);
		}
		if (v.Type == Variant::DataType::Long) {
			return floor(v.Long);
		}
		throw std::exception("Input must be a number.");
	};
	ctx.InternalFunctions["fmod"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 2)
			throw std::exception("Usage: fmod(a,b).");
		auto lft = vars[0];
		auto rht = vars[1];
		if (lft.Type == Variant::DataType::Double || rht.Type == Variant::DataType::Double) {
			return fmod(script_cast<double>(lft), script_cast<double>(rht));
		}
		if (lft.Type == Variant::DataType::Float || rht.Type == Variant::DataType::Float) {
			return fmod(script_cast<float>(lft), script_cast<float>(rht));
		}
		if (lft.Type == Variant::DataType::Long || rht.Type == Variant::DataType::Long) {
			return fmod(script_cast<long long>(lft), script_cast<long long>(rht));
		}
		if (lft.Type == Variant::DataType::Int || rht.Type == Variant::DataType::Int) {
			return fmod(script_cast<int>(lft), script_cast<int>(rht));
		}

		return {};
	};
	ctx.InternalFunctions["log"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 1)
			throw std::exception("Usage: log(x).");
		auto v = vars[0];
		if (v.Type == Variant::DataType::Double) {
			return log(v.Double);
		}
		if (v.Type == Variant::DataType::Float) {
			return log(v.Float);
		}
		if (v.Type == Variant::DataType::Int) {
			return log(v.Int);
		}
		if (v.Type == Variant::DataType::Long) {
			return log(v.Long);
		}
		throw std::exception("Input must be a number.");
	};
	ctx.InternalFunctions["log10"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 1)
			throw std::exception("Usage: log10(x).");
		auto v = vars[0];
		if (v.Type == Variant::DataType::Double) {
			return log10(v.Double);
		}
		if (v.Type == Variant::DataType::Float) {
			return log10(v.Float);
		}
		if (v.Type == Variant::DataType::Int) {
			return log10(v.Int);
		}
		if (v.Type == Variant::DataType::Long) {
			return log10(v.Long);
		}
		throw std::exception("Input must be a number.");
	};
	ctx.InternalFunctions["pow"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 2)
			throw std::exception("Usage: pow(a,b).");
		auto lft = vars[0];
		auto rht = vars[1];
		if (lft.Type == Variant::DataType::Double || rht.Type == Variant::DataType::Double) {
			return pow(script_cast<double>(lft), script_cast<double>(rht));
		}
		if (lft.Type == Variant::DataType::Float || rht.Type == Variant::DataType::Float) {
			return pow(script_cast<float>(lft), script_cast<float>(rht));
		}
		if (lft.Type == Variant::DataType::Long || rht.Type == Variant::DataType::Long) {
			return pow(script_cast<long long>(lft), script_cast<long long>(rht));
		}
		if (lft.Type == Variant::DataType::Int || rht.Type == Variant::DataType::Int) {
			return pow(script_cast<int>(lft), script_cast<int>(rht));
		}

		return {};
	};
	ctx.InternalFunctions["sin"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 1)
			throw std::exception("Usage: sin(x).");
		auto v = vars[0];
		if (v.Type == Variant::DataType::Double) {
			return sin(v.Double);
		}
		if (v.Type == Variant::DataType::Float) {
			return sin(v.Float);
		}
		if (v.Type == Variant::DataType::Int) {
			return sin(v.Int);
		}
		if (v.Type == Variant::DataType::Long) {
			return sin(v.Long);
		}
		throw std::exception("Input must be a number.");
	};
	ctx.InternalFunctions["sinh"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 1)
			throw std::exception("Usage: sinh(x).");
		auto v = vars[0];
		if (v.Type == Variant::DataType::Double) {
			return sinh(v.Double);
		}
		if (v.Type == Variant::DataType::Float) {
			return sinh(v.Float);
		}
		if (v.Type == Variant::DataType::Int) {
			return sinh(v.Int);
		}
		if (v.Type == Variant::DataType::Long) {
			return sinh(v.Long);
		}
		throw std::exception("Input must be a number.");
	};
	ctx.InternalFunctions["sqrt"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 1)
			throw std::exception("Usage: sqrt(x).");
		auto v = vars[0];
		if (v.Type == Variant::DataType::Double) {
			return sqrt(v.Double);
		}
		if (v.Type == Variant::DataType::Float) {
			return sqrt(v.Float);
		}
		if (v.Type == Variant::DataType::Int) {
			return sqrt(v.Int);
		}
		if (v.Type == Variant::DataType::Long) {
			return sqrt(v.Long);
		}
		throw std::exception("Input must be a number.");
	};
	ctx.InternalFunctions["tan"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 1)
			throw std::exception("Usage: tan(x).");
		auto v = vars[0];
		if (v.Type == Variant::DataType::Double) {
			return tan(v.Double);
		}
		if (v.Type == Variant::DataType::Float) {
			return tan(v.Float);
		}
		if (v.Type == Variant::DataType::Int) {
			return tan(v.Int);
		}
		if (v.Type == Variant::DataType::Long) {
			return tan(v.Long);
		}
		throw std::exception("Input must be a number.");
	};
	ctx.InternalFunctions["tanh"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 1)
			throw std::exception("Usage: tanh(x).");
		auto v = vars[0];
		if (v.Type == Variant::DataType::Double) {
			return tanh(v.Double);
		}
		if (v.Type == Variant::DataType::Float) {
			return tanh(v.Float);
		}
		if (v.Type == Variant::DataType::Int) {
			return tanh(v.Int);
		}
		if (v.Type == Variant::DataType::Long) {
			return tanh(v.Long);
		}
		throw std::exception("Input must be a number.");
	};
	ctx.InternalFunctions["acosh"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 1)
			throw std::exception("Usage: acosh(x).");
		auto v = vars[0];
		if (v.Type == Variant::DataType::Double) {
			return acosh(v.Double);
		}
		if (v.Type == Variant::DataType::Float) {
			return acosh(v.Float);
		}
		if (v.Type == Variant::DataType::Int) {
			return acosh(v.Int);
		}
		if (v.Type == Variant::DataType::Long) {
			return acosh(v.Long);
		}
		throw std::exception("Input must be a number.");
	};
	ctx.InternalFunctions["asinh"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 1)
			throw std::exception("Usage: asinh(x).");
		auto v = vars[0];
		if (v.Type == Variant::DataType::Double) {
			return asinh(v.Double);
		}
		if (v.Type == Variant::DataType::Float) {
			return asinh(v.Float);
		}
		if (v.Type == Variant::DataType::Int) {
			return asinh(v.Int);
		}
		if (v.Type == Variant::DataType::Long) {
			return asinh(v.Long);
		}
		throw std::exception("Input must be a number.");
	};
	ctx.InternalFunctions["atanh"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 1)
			throw std::exception("Usage: atanh(x).");
		auto v = vars[0];
		if (v.Type == Variant::DataType::Double) {
			return atanh(v.Double);
		}
		if (v.Type == Variant::DataType::Float) {
			return atanh(v.Float);
		}
		if (v.Type == Variant::DataType::Int) {
			return atanh(v.Int);
		}
		if (v.Type == Variant::DataType::Long) {
			return atanh(v.Long);
		}
		throw std::exception("Input must be a number.");
	};
	ctx.InternalFunctions["cbrt"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 1)
			throw std::exception("Usage: cbrt(x).");
		auto v = vars[0];
		if (v.Type == Variant::DataType::Double) {
			return cbrt(v.Double);
		}
		if (v.Type == Variant::DataType::Float) {
			return cbrt(v.Float);
		}
		if (v.Type == Variant::DataType::Int) {
			return cbrt(v.Int);
		}
		if (v.Type == Variant::DataType::Long) {
			return cbrt(v.Long);
		}
		throw std::exception("Input must be a number.");
	};
	ctx.InternalFunctions["erf"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 1)
			throw std::exception("Usage: erf(x).");
		auto v = vars[0];
		if (v.Type == Variant::DataType::Double) {
			return erf(v.Double);
		}
		if (v.Type == Variant::DataType::Float) {
			return erf(v.Float);
		}
		if (v.Type == Variant::DataType::Int) {
			return erf(v.Int);
		}
		if (v.Type == Variant::DataType::Long) {
			return erf(v.Long);
		}
		throw std::exception("Input must be a number.");
	};
	ctx.InternalFunctions["erfc"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 1)
			throw std::exception("Usage: erfc(x).");
		auto v = vars[0];
		if (v.Type == Variant::DataType::Double) {
			return erfc(v.Double);
		}
		if (v.Type == Variant::DataType::Float) {
			return erfc(v.Float);
		}
		if (v.Type == Variant::DataType::Int) {
			return erfc(v.Int);
		}
		if (v.Type == Variant::DataType::Long) {
			return erfc(v.Long);
		}
		throw std::exception("Input must be a number.");
	};
	ctx.InternalFunctions["expm1"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 1)
			throw std::exception("Usage: expm1(x).");
		auto v = vars[0];
		if (v.Type == Variant::DataType::Double) {
			return expm1(v.Double);
		}
		if (v.Type == Variant::DataType::Float) {
			return expm1(v.Float);
		}
		if (v.Type == Variant::DataType::Int) {
			return expm1(v.Int);
		}
		if (v.Type == Variant::DataType::Long) {
			return expm1(v.Long);
		}
		throw std::exception("Input must be a number.");
	};
	ctx.InternalFunctions["exp2"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 1)
			throw std::exception("Usage: exp2(x).");
		auto v = vars[0];
		if (v.Type == Variant::DataType::Double) {
			return exp2(v.Double);
		}
		if (v.Type == Variant::DataType::Float) {
			return exp2(v.Float);
		}
		if (v.Type == Variant::DataType::Int) {
			return exp2(v.Int);
		}
		if (v.Type == Variant::DataType::Long) {
			return exp2(v.Long);
		}
		throw std::exception("Input must be a number.");
	};
	ctx.InternalFunctions["lgamma"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 1)
			throw std::exception("Usage: lgamma(x).");
		auto v = vars[0];
		if (v.Type == Variant::DataType::Double) {
			return lgamma(v.Double);
		}
		if (v.Type == Variant::DataType::Float) {
			return lgamma(v.Float);
		}
		if (v.Type == Variant::DataType::Int) {
			return lgamma(v.Int);
		}
		if (v.Type == Variant::DataType::Long) {
			return lgamma(v.Long);
		}
		throw std::exception("Input must be a number.");
	};
	ctx.InternalFunctions["log1p"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 1)
			throw std::exception("Usage: log1p(x).");
		auto v = vars[0];
		if (v.Type == Variant::DataType::Double) {
			return log1p(v.Double);
		}
		if (v.Type == Variant::DataType::Float) {
			return log1p(v.Float);
		}
		if (v.Type == Variant::DataType::Int) {
			return log1p(v.Int);
		}
		if (v.Type == Variant::DataType::Long) {
			return log1p(v.Long);
		}
		throw std::exception("Input must be a number.");
	};
	ctx.InternalFunctions["log2"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 1)
			throw std::exception("Usage: log2(x).");
		auto v = vars[0];
		if (v.Type == Variant::DataType::Double) {
			return log2(v.Double);
		}
		if (v.Type == Variant::DataType::Float) {
			return log2(v.Float);
		}
		if (v.Type == Variant::DataType::Int) {
			return log2(v.Int);
		}
		if (v.Type == Variant::DataType::Long) {
			return log2(v.Long);
		}
		throw std::exception("Input must be a number.");
	};
	ctx.InternalFunctions["logb"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 1)
			throw std::exception("Usage: logb(x).");
		auto v = vars[0];
		if (v.Type == Variant::DataType::Double) {
			return logb(v.Double);
		}
		if (v.Type == Variant::DataType::Float) {
			return logb(v.Float);
		}
		if (v.Type == Variant::DataType::Int) {
			return logb(v.Int);
		}
		if (v.Type == Variant::DataType::Long) {
			return logb(v.Long);
		}
		throw std::exception("Input must be a number.");
	};
	ctx.InternalFunctions["nearbyint"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 1)
			throw std::exception("Usage: nearbyint(x).");
		auto v = vars[0];
		if (v.Type == Variant::DataType::Double) {
			return nearbyint(v.Double);
		}
		if (v.Type == Variant::DataType::Float) {
			return nearbyint(v.Float);
		}
		if (v.Type == Variant::DataType::Int) {
			return nearbyint(v.Int);
		}
		if (v.Type == Variant::DataType::Long) {
			return nearbyint(v.Long);
		}
		throw std::exception("Input must be a number.");
	};
	ctx.InternalFunctions["rint"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 1)
			throw std::exception("Usage: rint(x).");
		auto v = vars[0];
		if (v.Type == Variant::DataType::Double) {
			return rint(v.Double);
		}
		if (v.Type == Variant::DataType::Float) {
			return rint(v.Float);
		}
		if (v.Type == Variant::DataType::Int) {
			return rint(v.Int);
		}
		if (v.Type == Variant::DataType::Long) {
			return rint(v.Long);
		}
		throw std::exception("Input must be a number.");
	};
	ctx.InternalFunctions["fdim"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 2)
			throw std::exception("Usage: fdim(a,b).");
		auto lft = vars[0];
		auto rht = vars[1];
		if (lft.Type == Variant::DataType::Double || rht.Type == Variant::DataType::Double) {
			return fdim(script_cast<double>(lft), script_cast<double>(rht));
		}
		if (lft.Type == Variant::DataType::Float || rht.Type == Variant::DataType::Float) {
			return fdim(script_cast<float>(lft), script_cast<float>(rht));
		}
		if (lft.Type == Variant::DataType::Long || rht.Type == Variant::DataType::Long) {
			return fdim(script_cast<long long>(lft), script_cast<long long>(rht));
		}
		if (lft.Type == Variant::DataType::Int || rht.Type == Variant::DataType::Int) {
			return fdim(script_cast<int>(lft), script_cast<int>(rht));
		}

		return {};
	};
	ctx.InternalFunctions["fmax"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 2)
			throw std::exception("Usage: fmax(a,b).");
		auto lft = vars[0];
		auto rht = vars[1];
		if (lft.Type == Variant::DataType::Double || rht.Type == Variant::DataType::Double) {
			return fmax(script_cast<double>(lft), script_cast<double>(rht));
		}
		if (lft.Type == Variant::DataType::Float || rht.Type == Variant::DataType::Float) {
			return fmax(script_cast<float>(lft), script_cast<float>(rht));
		}
		if (lft.Type == Variant::DataType::Long || rht.Type == Variant::DataType::Long) {
			return fmax(script_cast<long long>(lft), script_cast<long long>(rht));
		}
		if (lft.Type == Variant::DataType::Int || rht.Type == Variant::DataType::Int) {
			return fmax(script_cast<int>(lft), script_cast<int>(rht));
		}

		return {};
	};
	ctx.InternalFunctions["fmin"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 2)
			throw std::exception("Usage: fmin(a,b).");
		auto lft = vars[0];
		auto rht = vars[1];
		if (lft.Type == Variant::DataType::Double || rht.Type == Variant::DataType::Double) {
			return fmin(script_cast<double>(lft), script_cast<double>(rht));
		}
		if (lft.Type == Variant::DataType::Float || rht.Type == Variant::DataType::Float) {
			return fmin(script_cast<float>(lft), script_cast<float>(rht));
		}
		if (lft.Type == Variant::DataType::Long || rht.Type == Variant::DataType::Long) {
			return fmin(script_cast<long long>(lft), script_cast<long long>(rht));
		}
		if (lft.Type == Variant::DataType::Int || rht.Type == Variant::DataType::Int) {
			return fmin(script_cast<int>(lft), script_cast<int>(rht));
		}

		return {};
	};
	ctx.InternalFunctions["round"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 1)
			throw std::exception("Usage: round(x).");
		auto v = vars[0];
		if (v.Type == Variant::DataType::Double) {
			return round(v.Double);
		}
		if (v.Type == Variant::DataType::Float) {
			return round(v.Float);
		}
		if (v.Type == Variant::DataType::Int) {
			return round(v.Int);
		}
		if (v.Type == Variant::DataType::Long) {
			return round(v.Long);
		}
		throw std::exception("Input must be a number.");
	};
	ctx.InternalFunctions["trunc"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 1)
			throw std::exception("Usage: trunc(x).");
		auto v = vars[0];
		if (v.Type == Variant::DataType::Double) {
			return trunc(v.Double);
		}
		if (v.Type == Variant::DataType::Float) {
			return trunc(v.Float);
		}
		if (v.Type == Variant::DataType::Int) {
			return trunc(v.Int);
		}
		if (v.Type == Variant::DataType::Long) {
			return trunc(v.Long);
		}
		throw std::exception("Input must be a number.");
	};
	ctx.InternalFunctions["remainder"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 2)
			throw std::exception("Usage: remainder(a,b).");
		auto lft = vars[0];
		auto rht = vars[1];
		if (lft.Type == Variant::DataType::Double || rht.Type == Variant::DataType::Double) {
			return remainder(script_cast<double>(lft), script_cast<double>(rht));
		}
		if (lft.Type == Variant::DataType::Float || rht.Type == Variant::DataType::Float) {
			return remainder(script_cast<float>(lft), script_cast<float>(rht));
		}
		if (lft.Type == Variant::DataType::Long || rht.Type == Variant::DataType::Long) {
			return remainder(script_cast<long long>(lft), script_cast<long long>(rht));
		}
		if (lft.Type == Variant::DataType::Int || rht.Type == Variant::DataType::Int) {
			return remainder(script_cast<int>(lft), script_cast<int>(rht));
		}

		return {};
	};
	ctx.InternalFunctions["copysign"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 2)
			throw std::exception("Usage: copysign(a,b).");
		auto lft = vars[0];
		auto rht = vars[1];
		if (lft.Type == Variant::DataType::Double || rht.Type == Variant::DataType::Double) {
			return copysign(script_cast<double>(lft), script_cast<double>(rht));
		}
		if (lft.Type == Variant::DataType::Float || rht.Type == Variant::DataType::Float) {
			return copysign(script_cast<float>(lft), script_cast<float>(rht));
		}
		if (lft.Type == Variant::DataType::Long || rht.Type == Variant::DataType::Long) {
			return copysign(script_cast<long long>(lft), script_cast<long long>(rht));
		}
		if (lft.Type == Variant::DataType::Int || rht.Type == Variant::DataType::Int) {
			return copysign(script_cast<int>(lft), script_cast<int>(rht));
		}

		return {};
	};
	ctx.InternalFunctions["tgamma"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 1)
			throw std::exception("Usage: tgamma(x).");
		auto v = vars[0];
		if (v.Type == Variant::DataType::Double) {
			return tgamma(v.Double);
		}
		if (v.Type == Variant::DataType::Float) {
			return tgamma(v.Float);
		}
		if (v.Type == Variant::DataType::Int) {
			return tgamma(v.Int);
		}
		if (v.Type == Variant::DataType::Long) {
			return tgamma(v.Long);
		}
		throw std::exception("Input must be a number.");
	};
	ctx.InternalFunctions["isfinite"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 1)
			throw std::exception("Usage: isfinite(x).");
		auto v = vars[0];
		if (v.Type == Variant::DataType::Double) {
			return isfinite(v.Double);
		}
		if (v.Type == Variant::DataType::Float) {
			return isfinite(v.Float);
		}
		if (v.Type == Variant::DataType::Int) {
			return true;
		}
		if (v.Type == Variant::DataType::Long) {
			return true;
		}
		throw std::exception("Input must be a number.");
	};
	ctx.InternalFunctions["isinf"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 1)
			throw std::exception("Usage: isinf(x).");
		auto v = vars[0];
		if (v.Type == Variant::DataType::Double) {
			return isinf(v.Double);
		}
		if (v.Type == Variant::DataType::Float) {
			return isinf(v.Float);
		}
		if (v.Type == Variant::DataType::Int) {
			return false;
		}
		if (v.Type == Variant::DataType::Long) {
			return false;
		}
		throw std::exception("Input must be a number.");
	};
	ctx.InternalFunctions["isnan"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 1)
			throw std::exception("Usage: isnan(x).");
		auto v = vars[0];
		if (v.Type == Variant::DataType::Double) {
			return isnan(v.Double);
		}
		if (v.Type == Variant::DataType::Float) {
			return isnan(v.Float);
		}
		if (v.Type == Variant::DataType::Int) {
			return false;
		}
		if (v.Type == Variant::DataType::Long) {
			return false;
		}
		throw std::exception("Input must be a number.");
	};
	ctx.InternalFunctions["isnormal"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 1)
			throw std::exception("Usage: isnormal(x).");
		auto v = vars[0];
		if (v.Type == Variant::DataType::Double) {
			return isnormal(v.Double);
		}
		if (v.Type == Variant::DataType::Float) {
			return isnormal(v.Float);
		}
		if (v.Type == Variant::DataType::Int) {
			return true;
		}
		if (v.Type == Variant::DataType::Long) {
			return true;
		}
		throw std::exception("Input must be a number.");
	};
	ctx.InternalFunctions["isgreater"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 2)
			throw std::exception("Usage: isgreater(a,b).");
		auto lft = vars[0];
		auto rht = vars[1];
		if (lft.Type == Variant::DataType::Double || rht.Type == Variant::DataType::Double) {
			return isgreater(script_cast<double>(lft), script_cast<double>(rht));
		}
		if (lft.Type == Variant::DataType::Float || rht.Type == Variant::DataType::Float) {
			return isgreater(script_cast<float>(lft), script_cast<float>(rht));
		}
		if (lft.Type == Variant::DataType::Long || rht.Type == Variant::DataType::Long) {
			return isgreater(script_cast<long long>(lft), script_cast<long long>(rht));
		}
		if (lft.Type == Variant::DataType::Int || rht.Type == Variant::DataType::Int) {
			return isgreater(script_cast<int>(lft), script_cast<int>(rht));
		}

		return {};
	};
	ctx.InternalFunctions["isgreaterequal"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 2)
			throw std::exception("Usage: isgreaterequal(a,b).");
		auto lft = vars[0];
		auto rht = vars[1];
		if (lft.Type == Variant::DataType::Double || rht.Type == Variant::DataType::Double) {
			return isgreaterequal(script_cast<double>(lft), script_cast<double>(rht));
		}
		if (lft.Type == Variant::DataType::Float || rht.Type == Variant::DataType::Float) {
			return isgreaterequal(script_cast<float>(lft), script_cast<float>(rht));
		}
		if (lft.Type == Variant::DataType::Long || rht.Type == Variant::DataType::Long) {
			return isgreaterequal(script_cast<long long>(lft), script_cast<long long>(rht));
		}
		if (lft.Type == Variant::DataType::Int || rht.Type == Variant::DataType::Int) {
			return isgreaterequal(script_cast<int>(lft), script_cast<int>(rht));
		}

		return {};
	};
	ctx.InternalFunctions["isless"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 2)
			throw std::exception("Usage: isless(a,b).");
		auto lft = vars[0];
		auto rht = vars[1];
		if (lft.Type == Variant::DataType::Double || rht.Type == Variant::DataType::Double) {
			return isless(script_cast<double>(lft), script_cast<double>(rht));
		}
		if (lft.Type == Variant::DataType::Float || rht.Type == Variant::DataType::Float) {
			return isless(script_cast<float>(lft), script_cast<float>(rht));
		}
		if (lft.Type == Variant::DataType::Long || rht.Type == Variant::DataType::Long) {
			return isless(script_cast<long long>(lft), script_cast<long long>(rht));
		}
		if (lft.Type == Variant::DataType::Int || rht.Type == Variant::DataType::Int) {
			return isless(script_cast<int>(lft), script_cast<int>(rht));
		}

		return {};
	};
	ctx.InternalFunctions["islessequal"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 2)
			throw std::exception("Usage: islessequal(a,b).");
		auto lft = vars[0];
		auto rht = vars[1];
		if (lft.Type == Variant::DataType::Double || rht.Type == Variant::DataType::Double) {
			return islessequal(script_cast<double>(lft), script_cast<double>(rht));
		}
		if (lft.Type == Variant::DataType::Float || rht.Type == Variant::DataType::Float) {
			return islessequal(script_cast<float>(lft), script_cast<float>(rht));
		}
		if (lft.Type == Variant::DataType::Long || rht.Type == Variant::DataType::Long) {
			return islessequal(script_cast<long long>(lft), script_cast<long long>(rht));
		}
		if (lft.Type == Variant::DataType::Int || rht.Type == Variant::DataType::Int) {
			return islessequal(script_cast<int>(lft), script_cast<int>(rht));
		}

		return {};
	};
	ctx.InternalFunctions["islessgreater"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 2)
			throw std::exception("Usage: islessgreater(a,b).");
		auto lft = vars[0];
		auto rht = vars[1];
		if (lft.Type == Variant::DataType::Double || rht.Type == Variant::DataType::Double) {
			return islessgreater(script_cast<double>(lft), script_cast<double>(rht));
		}
		if (lft.Type == Variant::DataType::Float || rht.Type == Variant::DataType::Float) {
			return islessgreater(script_cast<float>(lft), script_cast<float>(rht));
		}
		if (lft.Type == Variant::DataType::Long || rht.Type == Variant::DataType::Long) {
			return islessgreater(script_cast<long long>(lft), script_cast<long long>(rht));
		}
		if (lft.Type == Variant::DataType::Int || rht.Type == Variant::DataType::Int) {
			return islessgreater(script_cast<int>(lft), script_cast<int>(rht));
		}

		return {};
	};
	ctx.InternalFunctions["isunordered"] = [](ScriptContext& ctx, std::vector<Variant>& vars) -> Variant {
		if (vars.size() != 2)
			throw std::exception("Usage: isunordered(a,b).");
		auto lft = vars[0];
		auto rht = vars[1];
		if (lft.Type == Variant::DataType::Double || rht.Type == Variant::DataType::Double) {
			return isunordered(script_cast<double>(lft), script_cast<double>(rht));
		}
		if (lft.Type == Variant::DataType::Float || rht.Type == Variant::DataType::Float) {
			return isunordered(script_cast<float>(lft), script_cast<float>(rht));
		}
		if (lft.Type == Variant::DataType::Long || rht.Type == Variant::DataType::Long) {
			return isunordered(script_cast<long long>(lft), script_cast<long long>(rht));
		}
		if (lft.Type == Variant::DataType::Int || rht.Type == Variant::DataType::Int) {
			return isunordered(script_cast<int>(lft), script_cast<int>(rht));
		}

		return {};
	};
}