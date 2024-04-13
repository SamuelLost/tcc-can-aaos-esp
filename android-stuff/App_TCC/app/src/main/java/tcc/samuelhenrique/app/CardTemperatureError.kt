package tcc.samuelhenrique.app

import androidx.compose.runtime.Composable
import java.util.Locale

@Composable
fun CardTemperatureError(temperature: Float) {
    ExpandableCard(
        titleCard = "TEMP-01",
        description = { Description("Erro na leitura do sensor de temperatura") },
        gravity = { Gravity("Média") },
        currentReading = {
            CurrentReading(
                "%.1f ºC".format(
                    locale = Locale("pt", "BR"),
                    temperature
                )
            )
        }
    )
}