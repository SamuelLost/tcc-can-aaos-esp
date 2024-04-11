package tcc.samuelhenrique.app

import android.os.Bundle
import android.util.Log
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.font.Font
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import tcc.samuelhenrique.app.ui.theme.App_TCCTheme
import tcc.samuelhenrique.app.ui.theme.Background_App
import tcc.samuelhenrique.app.ui.theme.Background_Card
import tcc.samuelhenrique.app.ui.theme.Subtitulos
import tcc.samuelhenrique.app.ui.theme.Texto
import kotlin.math.PI
import kotlin.math.atan
import kotlin.math.pow
import kotlin.math.sqrt

var temperature = -273.15F
var axis = intArrayOf(-996, 15000, 10046)

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            App_TCCTheme {
                // A surface container using the 'background' color from the theme
                Column(
                    modifier = Modifier
                        .fillMaxSize()
                        .background(Background_App)
                        .padding(24.dp)
                ) {
                    Card(
                        shape = RoundedCornerShape(8.dp),
                        modifier = Modifier
                            .width(530.dp)
                            .height(400.dp)
                            .align(Alignment.End)
                            .verticalScroll(rememberScrollState()),
                        colors = CardDefaults.cardColors(
                            containerColor = Background_Card,
                            contentColor = Texto
                        )
                    )
                    {
                        Column(
                            modifier = Modifier
                                .fillMaxWidth()
                                .padding(start = 28.dp),
                            horizontalAlignment = Alignment.Start

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
                            ExpandableCard(titleCard = "TMP-01",
                                description = { Description("Erro na leitura do sensor de temperatura do motor") },
                                gravity = { Gravity("Média") },
                                currentReading = { CurrentReading(temperature.toString() + " °C") })
                            Spacer(modifier = Modifier.padding(6.dp))
                            ExpandableCard(titleCard = "ACC-01",
                                description = { Description("Erro na leitura do sensor acelerômetro") },
                                gravity = { Gravity("Média") },
                                // Imprimir o valor do sensor de aceleração como X: 0.0, Y: 0.0, Z: 0.0 usando joinToString
                                currentReading = {
                                    CurrentReading(
                                        "%.1f º".format(
                                            processAccelerometerData(axis)
                                        )
                                    )
                                })
                        }

                    }
                }
            }
        }
    }
}

@Composable
fun CardWithTitleAndSubtitle(title: String, subtitle: String) {
    Card(
        modifier = Modifier
            .fillMaxWidth()
            .padding(16.dp)
    ) {
        Column(
            modifier = Modifier.padding(16.dp)
        ) {
            Text(
                text = title,
            )
            Text(
                text = subtitle,
            )
        }
    }
}

fun processAccelerometerData(acc: IntArray): Double {
    // Calcular angulação do veículo em relação ao solo usando aceleração (roll)
    var x = acc[0].toDouble() / 16384F
    var y = acc[1].toDouble() / 16384F
    var z = acc[2].toDouble() / 16384F

    if (y == 0.0 && z == 0.0) {
        return 0.0
    }
    var roll = atan(
        (-1) * x / sqrt(
            y.pow(2) + z.pow(2)
        ) * 180 / PI
    )

    Log.d("MainActivity", "Roll: $roll")
    return roll

}


@Preview(showBackground = true)
@Composable
fun Preview() {
    App_TCCTheme {
        Card(
            shape = RoundedCornerShape(8.dp),
            modifier = Modifier
                .width(530.dp)
                .height(400.dp)
                .verticalScroll(rememberScrollState()),
            colors = CardDefaults.cardColors(
                containerColor = Background_Card,
                contentColor = Texto
            )
        )
        {
            Column(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(start = 28.dp),
                horizontalAlignment = Alignment.Start

            ) {
                Text(
                    text = "Estado do veículo",
                    fontSize = 24.sp,
                    modifier = Modifier
                        .padding(top = 24.dp)
                        .align(Alignment.Start)
                        .height(30.dp),
                    color = Texto,
                    fontFamily = FontFamily(
                        Font(R.font.poppins_bold)
                    )
                )
                Text(
                    text = "Diagnóstico",
                    fontSize = 16.sp,
                    modifier = Modifier
//                                    .padding(bottom = 38.dp)
                        .align(Alignment.Start),
                    color = Subtitulos,
                    fontFamily = FontFamily(
                        Font(R.font.poppins_regular)
                    )
                )

            }
        }
    }
}