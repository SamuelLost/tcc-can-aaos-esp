package tcc.samuelhenrique.app

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
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

var temperature = 90F
var axis = intArrayOf(-996, 15000, 10046)
var errors = false
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
                        .padding(27.dp)
                ) {
                    Row {
                        Text(
                            text = "Dashboard",
                            fontSize = 24.sp,
                            modifier = Modifier
                                .height(30.dp),
                            color = Texto,
                            fontFamily = FontFamily(
                                Font(R.font.poppins_bold)
                            )
                        )
                        Spacer(modifier = Modifier.padding(100.dp)
                        )
                        Card(
                            shape = RoundedCornerShape(8.dp),
                            modifier = Modifier
                                .width(505.dp)
                                .height(400.dp)
                                .align(Alignment.CenterVertically)
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
                                horizontalAlignment = if (errors) Alignment.CenterHorizontally else Alignment.Start
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
                                if(!errors) {
                                    CardTemperatureError(temperature)
                                    Spacer(modifier = Modifier.padding(6.dp))
                                    CardAccelerometerError(axis)
                                } else {
                                    NoErrors()
                            }

                        }
                    }

                }
            }
        }
    }
}

@Preview(showBackground = true)
@Composable
fun Preview() {
    App_TCCTheme {
    }
}}