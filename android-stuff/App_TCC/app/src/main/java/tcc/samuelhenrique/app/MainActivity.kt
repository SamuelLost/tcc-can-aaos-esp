package tcc.samuelhenrique.app

import android.car.Car
import android.car.hardware.CarPropertyValue
import android.car.hardware.property.CarPropertyManager
import android.os.Bundle
import android.util.Log
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxHeight
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.layout.wrapContentSize
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableFloatStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.text.font.Font
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import tcc.samuelhenrique.app.ui.composables.cards.CardAccelerometerError
import tcc.samuelhenrique.app.ui.composables.cards.CardTemperature
import tcc.samuelhenrique.app.ui.composables.cards.CardTemperatureError
import tcc.samuelhenrique.app.ui.composables.cards.processAcceleration
import tcc.samuelhenrique.app.ui.screens.NoErrors
import tcc.samuelhenrique.app.ui.theme.App_TCCTheme
import tcc.samuelhenrique.app.ui.theme.Background_App
import tcc.samuelhenrique.app.ui.theme.Background_Card
import tcc.samuelhenrique.app.ui.theme.Subtitulos
import tcc.samuelhenrique.app.ui.theme.Texto

var temperature by mutableFloatStateOf(0.0f)
    private set
var temperature_fault by mutableStateOf(false)
    private set
var temperature_fault_code by mutableStateOf("")
    private set
var PROP_INFO_TEMPERATURE: Int = 559943680
var PROP_FAULT_CODE_TEMPERATURE: Int = 554700801

var acceleration by mutableStateOf(intArrayOf(0, 0, 0))
    private set
var acc_fault by mutableStateOf(false)
    private set
var acc_fault_code by mutableStateOf("")
    private set
var PROP_INFO_ACCELEROMETER: Int = 557912066
var PROP_FAULT_CODE_ACCELEROMETER: Int = 554700803

var no_errors by mutableStateOf(!acc_fault && !temperature_fault)
    private set

class MainActivity : ComponentActivity() {

    private lateinit var mCar: Car
    private lateinit var mCarPropertyManager: CarPropertyManager

    override fun onCreate(savedInstanceState: Bundle?) {
        mCar = Car.createCar(this)
        mCarPropertyManager = mCar.getCarManager(Car.PROPERTY_SERVICE) as CarPropertyManager

        mCarPropertyManager.registerCallback(
            EngineTemperature(),
            PROP_INFO_TEMPERATURE,
            CarPropertyManager.SENSOR_RATE_FASTEST
        )

        mCarPropertyManager.registerCallback(
            FaultMessageTemperature(),
            PROP_FAULT_CODE_TEMPERATURE,
            CarPropertyManager.SENSOR_RATE_FASTEST
        )

        mCarPropertyManager.registerCallback(
            Accelerometer(),
            PROP_INFO_ACCELEROMETER,
            CarPropertyManager.SENSOR_RATE_FASTEST
        )

        mCarPropertyManager.registerCallback(
            FaultMessageAccelerometer(),
            PROP_FAULT_CODE_ACCELEROMETER,
            CarPropertyManager.SENSOR_RATE_FASTEST
        )

        super.onCreate(savedInstanceState)
        setContent {
            App_TCCTheme {
                App()
            }
        }
    }
}

@Composable
fun App() {
    Column(
        modifier = Modifier
            .fillMaxSize()
            .background(Background_App)
            .padding(27.dp)
    ) {
        Row(
            horizontalArrangement = Arrangement.SpaceBetween,
            modifier = Modifier.fillMaxWidth()
        ) {
            Column(
                modifier = Modifier.wrapContentSize(),
                //height = 400.dp width = 400.dp
                horizontalAlignment = Alignment.CenterHorizontally
            ) {
                Image(
                    painter = painterResource(
                        id = when {
                            temperature_fault && acc_fault -> R.drawable.carro_acc_temp_v2
                            temperature_fault -> R.drawable.carro_temp_v2
                            acc_fault -> R.drawable.carro_acc_v2
                            else -> R.drawable.carro_v2
                        }
                    ),
                    contentDescription = "",
                    modifier = Modifier.size(
                        width = 400.dp,
                        height = 200.dp
                    )
                )
                Column(
                    verticalArrangement = Arrangement.Bottom,
                    modifier = Modifier.fillMaxHeight()
                ) {
                    CardTemperature(temperature)
                }
            }
            Spacer(modifier = Modifier.padding(10.dp))
            Card(
                shape = RoundedCornerShape(8.dp),
                modifier = Modifier
                    .width(500.dp)
                    .height(400.dp)
                    .align(Alignment.CenterVertically)
                    .verticalScroll(rememberScrollState()),
                colors = CardDefaults.cardColors(
                    containerColor = Background_Card,
                    contentColor = Texto
                )
            ) {
                Column(
                    modifier = Modifier
                        .fillMaxWidth()
                        .padding(start = 20.dp),
                    horizontalAlignment = if (!acc_fault && !temperature_fault) Alignment.CenterHorizontally else Alignment.Start
                ) {
                    Text(
                        text = "Estado do veículo",
                        fontSize = 24.sp,
                        modifier = Modifier
                            .padding(top = 24.dp)
                            .height(30.dp)
                            .align(Alignment.Start),
                        color = Texto,
                        fontFamily = FontFamily(
                            Font(R.font.poppins_bold)
                        )
                    )
                    Text(
                        text = "Diagnóstico",
                        fontSize = 16.sp,
                        modifier = Modifier
                            .padding(bottom = 38.dp)
                            .align(Alignment.Start),
                        color = Subtitulos,
                        fontFamily = FontFamily(
                            Font(R.font.poppins_regular)
                        )
                    )
//                    if (!no_errors) {
//                        CardTemperatureError(temperature)
//                        Spacer(modifier = Modifier.padding(6.dp))
//                        CardAccelerometerError(acceleration)
//                    } else {
//                        NoErrors()
//                    }
                    if (temperature_fault && acc_fault) {
                        CardTemperatureError(temperature = temperature, temperatureFaultCode = temperature_fault_code)
                        Spacer(modifier = Modifier.padding(6.dp))
                        CardAccelerometerError(axis = acceleration, accFaultCode = acc_fault_code)
                    } else {
                        if (temperature_fault) {
                            CardTemperatureError(temperature = temperature, temperatureFaultCode = temperature_fault_code)
                        } else if (acc_fault) {
                            CardAccelerometerError(axis = acceleration, accFaultCode = acc_fault_code)
                        } else {
                            NoErrors()
                        }
                    }

                }
            }
        }
    }
}

class EngineTemperature : CarPropertyManager.CarPropertyEventCallback {

    override fun onChangeEvent(prop: CarPropertyValue<*>) {
        val newTemperature = prop.value as? Float ?: return
        temperature = newTemperature
        temperature_fault = temperature <= -273
//        Log.i("Temperature", "$temperature ºC")
    }

    override fun onErrorEvent(p0: Int, p1: Int) {
//        TODO("Not yet implemented")
        temperature_fault = true
    }
}

class FaultMessageTemperature : CarPropertyManager.CarPropertyEventCallback {
    override fun onChangeEvent(prop: CarPropertyValue<*>) {
        val newTemperatureFaultCode = prop.value as? String ?: return
        temperature_fault_code = newTemperatureFaultCode
//        Log.i("Temperature Fault", temperature_fault_code)

    }
    override fun onErrorEvent(p0: Int, p1: Int) {
        TODO("Not yet implemented")
    }
}

class Accelerometer : CarPropertyManager.CarPropertyEventCallback {
    override fun onChangeEvent(prop: CarPropertyValue<*>) {
        val integerArray = prop.value as? Array<Int>
        if (integerArray != null) {
            acceleration = integerArray.toIntArray()
            acc_fault = acceleration[0] == -1 && acceleration[1] == -1 && acceleration[2] == -1
            processAcceleration(acceleration).toString()
        } else {
            Log.e("Acceleration", "Valor recebido não é um Array<Int>!")
        }
    }

    override fun onErrorEvent(p0: Int, p1: Int) {
        acc_fault = true
    }
}

class FaultMessageAccelerometer : CarPropertyManager.CarPropertyEventCallback {
    override fun onChangeEvent(prop: CarPropertyValue<*>) {
        val newAccFaultCode = prop.value as? String ?: return
        acc_fault_code = newAccFaultCode
//        Log.i("Acceleration Fault", acc_fault_code)
    }
    override fun onErrorEvent(p0: Int, p1: Int) {
        TODO("Not yet implemented")
    }
}


@Preview(showBackground = true, widthDp = 1024, heightDp = 450,
    device = "id:automotive_1024p_landscape"
)
@Composable
fun Preview() {
    App_TCCTheme {
        App()
    }
}