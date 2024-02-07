$funs = [System.IO.File]::ReadAllLines("cmath_functions.txt");
$sb = [System.Text.StringBuilder]::new();
$sb.Append("void LoadCMath(ScriptContext& ctx){");
foreach($fun in $funs){
    $vars = $fun -split ',';
    if ($vars[1] -eq "1"){
        $sb.AppendLine(@"
        ctx.InternalFunctions["FUN_MACRO"] = [](ScriptContext& ctx, std::vector<Variant> vars) -> Variant {
		if (vars.size() != 1)
			throw std::exception("Usage: FUN_MACRO(x).");
		auto v = vars[0];
		if (v.Type == Variant::DataType::Double){
			return FUN_MACRO(v.Double);
		}
		if (v.Type == Variant::DataType::Float) {
			return FUN_MACRO(v.Float);
		}
		if (v.Type == Variant::DataType::Int) {
			return FUN_MACRO(v.Int);
		}
		if (v.Type == Variant::DataType::Long) {
			return FUN_MACRO(v.Long);
		}
		throw std::exception("Input must be a number.");
	};
"@.Replace("FUN_MACRO",$vars[0]));
    }else{ 
        if ($vars[1] -eq "2") {
                    $sb.AppendLine(@"
	ctx.InternalFunctions["FUN_MACRO"] = [](ScriptContext& ctx, std::vector<Variant> vars) -> Variant {
		if (vars.size() != 2)
			throw std::exception("Usage: FUN_MACRO(a,b).");
		auto lft = vars[0];
		auto rht = vars[1];
		if (lft.Type == Variant::DataType::Double || rht.Type == Variant::DataType::Double) {
			return FUN_MACRO(script_cast<double>(lft), script_cast<double>(rht));
		}
		if (lft.Type == Variant::DataType::Float || rht.Type == Variant::DataType::Float) {
			return FUN_MACRO(script_cast<float>(lft), script_cast<float>(rht));
		}
		if (lft.Type == Variant::DataType::Long || rht.Type == Variant::DataType::Long) {
			return FUN_MACRO(script_cast<long long>(lft), script_cast<long long>(rht));
		}
		if (lft.Type == Variant::DataType::Int || rht.Type == Variant::DataType::Int) {
			return FUN_MACRO(script_cast<int>(lft), script_cast<int>(rht));
		}

		return {};
	};
"@.Replace("FUN_MACRO",$vars[0]));
        }
    }
}
$sb.Append("}");
[System.IO.File]::WriteAllText("CMathBulitins.h",$sb.ToString());