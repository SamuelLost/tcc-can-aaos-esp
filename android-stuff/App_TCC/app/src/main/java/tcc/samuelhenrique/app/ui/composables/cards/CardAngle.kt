package tcc.samuelhenrique.app.ui.composables.cards

import android.annotation.SuppressLint
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableDoubleStateOf
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.font.Font
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import tcc.samuelhenrique.app.R
import tcc.samuelhenrique.app.temperature
import tcc.samuelhenrique.app.ui.theme.Azul_Temperatura
import tcc.samuelhenrique.app.ui.theme.Background_Card
import tcc.samuelhenrique.app.ui.theme.Erro_Vermelho
import tcc.samuelhenrique.app.ui.theme.Subtitulos
import tcc.samuelhenrique.app.ui.theme.Texto
import java.util.Locale

@SuppressLint("UnrememberedMutableState")
@Composable
fun CardAngle(acc: IntArray) {
    var roll: Double by mutableDoubleStateOf(processAcceleration(acc))

    Card(
        shape = RoundedCornerShape(16.dp),
        colors = CardDefaults.cardColors(
            containerColor = Background_Card,
            contentColor = Texto
        ),
        modifier = Modifier
            .width(200.dp)
            .height(120.dp)

    ) {
        Text(
            text = "Inclinação",
            fontSize = 18.sp,
            fontFamily = FontFamily(
                Font(R.font.poppins_bold)
            ),
            color = Azul_Temperatura,
            modifier = Modifier
                .padding(start = 10.dp, top = 6.dp)
                .height(25.dp)
                .align(Alignment.Start)
        )
        Text(
            text = "Veículo",
            fontSize = 14.sp,
            fontFamily = FontFamily(
                Font(R.font.poppins_regular)
            ),
            color = Subtitulos,
            modifier = Modifier
                .align(Alignment.Start)
                .padding(start = 10.dp)
        )

        Text(
            text = "%.1f °".format(
                locale = Locale("pt", "BR"),
                roll),
            fontSize = 40.sp,
            fontFamily = FontFamily(
                Font(R.font.poppins_medium)
            ),
            color = if (temperature > 100) Erro_Vermelho else Azul_Temperatura,
            modifier = Modifier
                .align(Alignment.CenterHorizontally)
        )
    }
}

@Composable
@Preview(
    showBackground = true,
    device = "id:automotive_1024p_landscape"
)
fun CardAnglePreview() {
    CardAngle(intArrayOf(-1205, -10000, -15200))
}